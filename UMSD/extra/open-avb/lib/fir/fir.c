#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "fir.h"


/* ========== Definition for debugging ========== */
#undef SLEEP_BTW_CMD // jay.choi.debugging
#ifdef SLEEP_BTW_CMD
#define TIME_BTW_CMD 1000*1000
#endif

#undef SKIP_TCAM_SET_MODE // jay.choi.debugging
#define DO_NOT_SET_Q_MODE_PORT_9 // jay.choi.debugging
/* ========== Definition for debugging ========== */


#define FIR_MAX_PORTS 10
#define FIR_CMD_MAX_SIZE 1024

#define FIR_CHAR_BRACE_OPEN "{"
#define FIR_CHAR_BRACE_CLOSE "}"

#define FIR_CMD_VTU "vlan"
#define FIR_CMD_ATU "atu"
#define FIR_CMD_PORT "port"
#define FIR_CMD_Q_CTRL "qc"
#define FIR_CMD_QBV "qbv"
#define FIR_CMD_TCAM "tcam"
#define FIR_CMD_FRER "frer"

#define FIR_ADD_ENTRY "addEntry"
#define FIR_FIND_ENTRY "findEntry"
#define FIR_DEL_ENTRY "delEntry"
#define FIR_DEL_ALL_ENTRY "delAll"

#define FIR_GET_MODE "getMode"
#define FIR_SET_MODE "setMode"

#define FIR_GET_Q_MODE "getQMode"
#define FIR_SET_Q_MODE "setQMode"

#define FIR_GET_Q_CTRL "getQc"
#define FIR_SET_Q_CTRL "setQc"

#define FIR_WRITE "write"
#define FIR_READ "read"

#define FIR_GET_PORT_VLAN_MAP "getPortBasedVlanMap"
#define FIR_SET_PORT_VLAN_MAP "setPortBasedVlanMap"

#define FIR_SEQ_RCVY_FLUSH_ALL "seqRcvyFlushAll"
#define FIR_SEQ_RCVY_READ_CLEAR "seqRcvyReadClear"
#define FIR_SEQ_RCVY_ADD_ENTRY "seqRcvyAddEntry"

#define FIR_BANK1_SCTR_FLUSH_ALL "bank1SCtrFlushAll"
#define FIR_BANK1_SCTR_LOAD "bank1SCtrLoad"

#define FIR_OPTION_FID "-fid" // Forwarding Information Database
#define FIR_OPTION_VID "-vid" // VLAN Identifier
#define FIR_OPTION_TAG_P "-TagP"
#define FIR_OPTION_MAC_ADDR "-macAddr"
#define FIR_OPTION_STATE "-state"
#define FIR_OPTION_PORT_VECTOR "-portVec"
#define FIR_OPTION_Q_PRIORITY "-Qpri"

#define FIR_OPTION_SPV "-spv"
#define FIR_OPTION_SPV_MASK "-spvMask"
#define FIR_OPTION_FRAME_OCTET "-frameOctet"
#define FIR_OPTION_FRAME_OCTET_MASK "-frameOctetMask"
#define FIR_OPTION_DPV_MODE "-dpvMode"
#define FIR_OPTION_DPV_DATA "-dpvdata"

#define FIR_OPTION_FRER_SEQ_GEN_EN "-frerSeqGenEn"
#define FIR_OPTION_FRER_INDV_RCVY_ID "-frerIndvRcvyId"

#define FIR_OPTION_FRER_SCTR_INC "-frerSctrInc"
#define FIR_OPTION_FRER_SCTR_INDEX "-frerSctrIndex"
#define FIR_OPTION_FRER_SEQ_ENC_TYPE "-frerSeqEncType"
#define FIR_OPTION_FRER_SEQ_RCVY_EN "-frerSeqRcvyEn"
#define FIR_OPTION_FRER_SEQ_RCVY_INDEX "-frerSeqRcvyIndex"

#define FIR_OPTION_SEQ_RCVY_EN "-seqRcvyEN"
#define FIR_OPTION_SEQ_RCVY_PORT "-seqRcvyPort"
#define FIR_OPTION_SEQ_RCVY_INDEX "-seqRcvyIndex"
#define FIR_OPTION_SEQ_RCVY_ALG "-seqRcvyAlg"

#define FIR_OPTION_BANK1_SCTR_EN "-bk1SctrEn"
#define FIR_OPTION_BANK1_SCTR_PORT "-bk1SctrPort"
#define FIR_OPTION_BANK1_SCTR_INDEX "-bk1SctrIndex"


unsigned fir_port_left;
unsigned fir_port_right;
unsigned fir_port_bottom;

int fir_sock = SOCKET_ERROR;


int fir_connect(void)
{
	struct sockaddr_in addr;

	fir_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fir_sock < 0)
		goto out;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(MCLI_PORT_DEFAULT);
	inet_aton("127.0.0.1", &addr.sin_addr);

	printf("fir_connect()\n");

	return SOCKET_OK;

out:	
	if (fir_sock != SOCKET_ERROR)
		close(fir_sock);
	fir_sock = SOCKET_ERROR;

	return SOCKET_ERROR;
}


void fir_disconnect(void)
{
	if (fir_sock != SOCKET_ERROR)
		close(fir_sock);
}


int fir_send_data(char *data, int len)
{
	struct sockaddr_in addr;
	socklen_t addr_len;

#ifdef SLEEP_BTW_CMD 
	fir_sleep();
#endif 

	printf("fir_send_data(): %s, %d\n", data, len);

	if (fir_sock == SOCKET_ERROR)
		return SOCKET_ERROR;

	if (data == NULL)
		return SOCKET_ERROR;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(MCLI_PORT_DEFAULT);
	inet_aton("127.0.0.1", &addr.sin_addr);
	addr_len = sizeof(addr);

	return sendto(fir_sock, data, len, 0, (struct sockaddr *)&addr, addr_len);
}


/* vlan delAll */
/* vlan addEntry -fid 0x1 -vid 0x1 -TagP {3 3 2 3 3 3 2 3 3 3} */
/* vlan addEntry -fid 0x2 -vid 0x5 -TagP {3 3 3 2  3 3 2 3 3 2} */
int fir_cfg_vtu(U8 fid, U8 vid, U8 portTag[])
{
	int rc = SOCKET_ERROR;
	char *msgBuf;

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		return SOCKET_ERROR;
	}

	msgBuf = malloc(FIR_CMD_MAX_SIZE);
	if (NULL == msgBuf) 
	{
		printf("malloc failed\n");
		return SOCKET_ERROR;
	}

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// vlan delAll
	sprintf(msgBuf, "%s %s", FIR_CMD_VTU, FIR_DEL_ALL_ENTRY); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(rc != SOCKET_OK)
	{
		if(msgBuf)
		{
			free(msgBuf);
		}	
		return rc;
	}

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	//vlan addEntry -fid 0x1 -vid 0x1 -TagP {3 3 2 3 3 3 2 3 3 3}
	// vlan addEntry -fid 0x2 -vid 0x5 -TagP {3 3 3 2  3 3 2 3 3 2}
	sprintf(msgBuf, "%s %s %s 0x%x %s 0x%x %s %s%d %d %d %d %d %d %d %d %d %d%s",
		FIR_CMD_VTU, FIR_ADD_ENTRY, 
		FIR_OPTION_FID, fid,
		FIR_OPTION_VID, vid,
		FIR_OPTION_TAG_P, FIR_CHAR_BRACE_OPEN,
		portTag[0], portTag[1], portTag[2], portTag[3], portTag[4], portTag[5], portTag[6], portTag[7], portTag[8], portTag[9], 
		FIR_CHAR_BRACE_CLOSE); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR; 
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(msgBuf)
	{
		free(msgBuf);
	}

	return (rc);
}	


/* atu delAll 1 */
/* atu addEntry -macAddr 00005a1d132E -state 0xb -fid 0x1 -portVec 0x40 -Qpri 0x2 */
/* atu addEntry -macAddr 002222222222 -state 0xE -portVec 0x200 */
int fir_cfg_atu(U8 flush_all, char *macAddr, U8 entryState, U8 fid, U16 portVec, U8 macQPri)
{
	int rc = SOCKET_ERROR;
	char *msgBuf;

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		return SOCKET_ERROR;
	}

	msgBuf = malloc(FIR_CMD_MAX_SIZE);
	if (NULL == msgBuf) 
	{
		printf("malloc failed\n");
		return SOCKET_ERROR;
	}

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// atu delAll
	sprintf(msgBuf, "%s %s %d", FIR_CMD_ATU, FIR_DEL_ALL_ENTRY, flush_all); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(rc != SOCKET_OK)
	{
		if(msgBuf)
		{
			free(msgBuf);
		}	
		return rc;
	}

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	//atu addEntry -macAddr 00005a1d132e -state 0xb -fid 0x1 -portVec 0x40 -Qpri 0x2
	sprintf(msgBuf, "%s %s %s %s %s 0x%x %s 0x%x %s 0x%x %s 0x%x",
		FIR_CMD_ATU, FIR_ADD_ENTRY, 
		FIR_OPTION_MAC_ADDR, macAddr, 
		FIR_OPTION_STATE, entryState,
		FIR_OPTION_FID, fid,
		FIR_OPTION_PORT_VECTOR, portVec,
		FIR_OPTION_Q_PRIORITY, macQPri); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(msgBuf)
	{
		free(msgBuf);
	}
	
	return (rc);
}	


/* port setQMode 2 2 */
/* port setQMode 6 2 */ 
int fir_cfg_portQMode(U8 portNum, U8 qMode)
{
	int rc = SOCKET_ERROR;
	char *msgBuf;

#ifdef DO_NOT_SET_Q_MODE_PORT_9
	if(portNum == 0x9)
	{
		printf("fir_cfg_portQMode(%d, %d): DO_NOT_SET_Q_MODE_PORT_9\n", portNum, qMode);
		return SOCKET_OK;
	}
#endif

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		return SOCKET_ERROR;
	}

	msgBuf = malloc(FIR_CMD_MAX_SIZE);
	if (NULL == msgBuf) 
	{
		printf("malloc failed\n");
		return SOCKET_ERROR;
	}

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// port setQMode 2 2
	// port setQMode 6 2
	sprintf(msgBuf, "%s %s %d %d", FIR_CMD_PORT, FIR_SET_Q_MODE, portNum, qMode); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(msgBuf)
	{
		free(msgBuf);
	}
	
	return (rc);
}


/* qc setQc 6 0 3 */
int fir_cfg_portQCtrl(U8 port, U8 pointer, U8 mode)
{
	int rc = SOCKET_ERROR;
	char *msgBuf;

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		return SOCKET_ERROR;
	}

	msgBuf = malloc(FIR_CMD_MAX_SIZE);
	if (NULL == msgBuf) 
	{
		printf("malloc failed\n");
		return SOCKET_ERROR;
	}

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// qc setQc 6 0 3
	sprintf(msgBuf, "%s %s %d %d %d", FIR_CMD_Q_CTRL, FIR_SET_Q_CTRL, port, pointer, mode); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed : %d, %d\n", msgBuf, rc, strlen(msgBuf));
	}

	if(msgBuf)
	{
		free(msgBuf);
	}

	return (rc);
}


/* qbv write 0x1 0x1f 0x0 0x3322 */	
/* qbv write 0x1 0x1f 0x4 0x0000 */	
/* qbv write 0x1 0x2 0x0 0x8000 */
int fir_cfg_avbPolicy(U8 tsnPort, U8 regOffset, U16 data)
{
	int rc = SOCKET_ERROR;
	char *msgBuf;
	int tsnBlock = 0x1; // AVB policy

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		return SOCKET_ERROR;
	}

	msgBuf = malloc(FIR_CMD_MAX_SIZE);
	if (NULL == msgBuf) 
	{
		printf("malloc failed\n");
		return SOCKET_ERROR;
	}

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// qbv write 0x1 0x1f 0x0 0x3322	
	// qbv write 0x1 0x1f 0x4 0x0000	
	// qbv write 0x1 0x2 0x0 0x8000
	sprintf(msgBuf, "%s %s 0x%x 0x%x 0x%x 0x%x", FIR_CMD_QBV, FIR_WRITE, tsnBlock, tsnPort, regOffset, data); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(msgBuf)
	{
		free(msgBuf);
	}

	return (rc);
} 


/* qbv write 0x2 0x1f 0x1 0x0064 */
/* qbv write 0x2 0x1f 0x0 0x8064 */
/* qbv write 0x2 0x6 0x4 0x0509 */
int fir_cfg_qav(U8 tsnPort, U8 regOffset, U16 data)
{
	int rc = SOCKET_ERROR;
	char *msgBuf;
	int tsnBlock = 0x2; // Qav

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		return SOCKET_ERROR;
	}

	msgBuf = malloc(FIR_CMD_MAX_SIZE);
	if (NULL == msgBuf) 
	{
		printf("malloc failed\n");
		return SOCKET_ERROR;
	}

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// qbv write 0x2 0x1f 0x1 0x0064
	// qbv write 0x2 0x1f 0x0 0x8064
	// qbv write 0x2 0x6 0x4 0x0509
	sprintf(msgBuf, "%s %s 0x%x 0x%x 0x%x 0x%x", FIR_CMD_QBV, FIR_WRITE, tsnBlock, tsnPort, regOffset, data); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(msgBuf)
	{
		free(msgBuf);
	}
	
	return (rc);
}  


/* tcam setMode 9 1 */
int fir_cfg_tcamMode(U8 portNum, U8 tcamMode)
{
	int rc = SOCKET_ERROR;
	char *msgBuf;

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		return SOCKET_ERROR;
	}

	msgBuf = malloc(FIR_CMD_MAX_SIZE);
	if (NULL == msgBuf) 
	{
		printf("malloc failed\n");
		return SOCKET_ERROR;
	}

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// tcam setMode 9 1
	sprintf(msgBuf, "%s %s %d %d", FIR_CMD_TCAM, FIR_SET_MODE, portNum, tcamMode); 

#ifdef SKIP_TCAM_SET_MODE
	if(tcamMode == 0x1)
	{
		printf("fir_cfg_tcamMode is skipped : %s\n", msgBuf);
		if(msgBuf)
		{
			free(msgBuf);
		}
		return SOCKET_OK;
	}
#endif

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(msgBuf)
	{
		free(msgBuf);
	}
	
	return (rc);
}

/* tcam delAll ?? */
/* tcam addEntry 0 
  -spv ? -spvMask ? 
  -frameOctet ? -frameOctetmask ?
  -dpvMode ? -dpvdata ? (gen)
  -frerSeqGenEn ? -frerIndvRcvyId ? (gen)
  -frerSctrInc ? -frerSctrIndex ? (rcvy)
  -frerSeqEncType ? -frerSeqRcvyEn ? -frerSeqRcvyIndex ? (rcvy) 
  */
/* tcam addEntry 0 -spv 0x200 -spvMask 0x3FF -frameOctet { 0 0x22 } -frameOctetmask { 0 0xFF } -dpvMode 3 -dpvdata 0x48 -frerSeqGenEn 1 -frerIndvRcvyId 0xF */
/* tcam addEntry 0 -spvMask 0x3F3 -spv 0x0 -frameOctet { 0 0x22 } -frameOctetmask { 0 0xFF } -frerSctrInc 1 -frerSctrIndex 0xF -frerSeqEncType 1 -frerSeqRcvyEn 1 -frerSeqRcvyIndex 0xF */
int fir_cfg_tcam(U8 entryNum, 
	U16 spv, U16 spvMask, 
	U8 frameOctet[], U8 frameOctetMask[], 
	U8 dpvMode, U32 dpvData, /* Replication */
	U8 frerSeqGenEn, U8 frerIndvRcvyId, /* Replication */
	U8 frerSctrInc, U8 frerSctrIndex, /* Elimination */
	U8 frerSeqEncType, U8 frerSeqRcvyEn, U8 frerSeqRcvyIndex) /* Elimination */
{
	int rc = SOCKET_ERROR;
	char *msgBuf;

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		return SOCKET_ERROR;
	}

	msgBuf = malloc(FIR_CMD_MAX_SIZE);
	if (NULL == msgBuf) 
	{
		printf("malloc failed\n");
		return SOCKET_ERROR;
	}

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// tcam delAll
	sprintf(msgBuf, "%s %s", FIR_CMD_TCAM, FIR_DEL_ALL_ENTRY); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(rc != SOCKET_OK)
	{
		if(msgBuf)
		{
			free(msgBuf);
		}	
		return rc;
	} 

	//tcam addEntry 0 -spv 0x200 -spvMask 0x3FF -frameOctet { 0 0x22 } -frameOctetmask { 0 0xFF } -dpvMode 3 -dpvdata 0x48 -frerSeqGenEn 1 -frerIndvRcvyId 0xF
	sprintf(msgBuf, "%s %s %d %s 0x%x %s 0x%x %s %s 0x%x 0x%x %s %s %s 0x%x 0x%x %s %s 0x%x %s 0x%x %s 0x%x %s 0x%x %s 0x%x %s 0x%x %s 0x%x %s 0x%x %s 0x%x", FIR_CMD_TCAM, FIR_ADD_ENTRY, entryNum, FIR_OPTION_SPV, spv, 
		FIR_OPTION_SPV_MASK, spvMask,
		FIR_OPTION_FRAME_OCTET, FIR_CHAR_BRACE_OPEN, frameOctet[0], frameOctet[1], FIR_CHAR_BRACE_CLOSE, 
		FIR_OPTION_FRAME_OCTET_MASK, FIR_CHAR_BRACE_OPEN, frameOctetMask[0], frameOctetMask[1], FIR_CHAR_BRACE_CLOSE, 
		FIR_OPTION_DPV_MODE, dpvMode,
		FIR_OPTION_DPV_DATA, dpvData,
		FIR_OPTION_FRER_SEQ_GEN_EN, frerSeqGenEn,
		FIR_OPTION_FRER_INDV_RCVY_ID, frerIndvRcvyId, 
		FIR_OPTION_FRER_SCTR_INC, frerSctrInc,
		FIR_OPTION_FRER_SCTR_INDEX, frerSctrIndex, 
		FIR_OPTION_FRER_SEQ_ENC_TYPE, frerSeqEncType,
		FIR_OPTION_FRER_SEQ_RCVY_EN, frerSeqRcvyEn,
		FIR_OPTION_FRER_SEQ_RCVY_INDEX, frerSeqRcvyIndex); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(msgBuf)
	{
		free(msgBuf);
	}
	
	return (rc);
}


/* port setPortBasedVlanMap 2 {0 1 4 5 6 7 8 9} */
/* port setPortBasedVlanMap 3 {0 1 4 5 6 7 8 9} */
int fir_cfg_portVlanMap(U8 ingress, U8 portMap[])
{
	int rc = SOCKET_ERROR;
	char *msgBuf;

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		return SOCKET_ERROR;
	}

	msgBuf = malloc(FIR_CMD_MAX_SIZE);
	if (NULL == msgBuf) 
	{
		printf("malloc failed\n");
		return SOCKET_ERROR;
	}

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// port setPortBasedVlanMap 2 {0 1 4 5 6 7 8 9}
	// port setPortBasedVlanMap 3 {0 1 4 5 6 7 8 9}
	sprintf(msgBuf, "%s %s %d %s%d %d %d %d %d %d %d %d%s",
		FIR_CMD_PORT, FIR_SET_PORT_VLAN_MAP, ingress,
		FIR_CHAR_BRACE_OPEN, portMap[0], portMap[1], portMap[2], portMap[3], portMap[4], portMap[5], portMap[6], portMap[7], 
		FIR_CHAR_BRACE_CLOSE); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(msgBuf)
	{
		free(msgBuf);
	}
	
	return (rc);
}


/* frer seqRcvyFlushAll */
/* frer seqRcvyReadClear 0 : 0 = SeqRcvyId */
/* frer seqRcvyAddEntry 0 -seqRcvyEN 1 -seqRcvyPort 7 -seqRcvyIndex 0xf -seqRcvyAlg 1 */
int fir_cfg_seqRcvy(U8 seqRcvyId, U8 seqRcvyEn, U8 seqRcvyPort, U8 seqRcvyIndex, U8 seqRcvyAlg)
{
	int rc = SOCKET_ERROR;
	char *msgBuf;

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		return SOCKET_ERROR;
	}

	msgBuf = malloc(FIR_CMD_MAX_SIZE);
	if (NULL == msgBuf) 
	{
		printf("malloc failed\n");
		return SOCKET_ERROR;
	}

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// frer seqRcvyFlushAll
	sprintf(msgBuf, "%s %s", FIR_CMD_FRER, FIR_SEQ_RCVY_FLUSH_ALL); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(rc != SOCKET_OK)
	{
		if(msgBuf)
		{
			free(msgBuf);
		}	
		return rc;
	} 

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// frer seqRcvyReadClear 0
	sprintf(msgBuf, "%s %s %d", FIR_CMD_FRER, FIR_SEQ_RCVY_READ_CLEAR, seqRcvyId); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(rc != SOCKET_OK)
	{
		if(msgBuf)
		{
			free(msgBuf);
		}	
		return rc;
	}  

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// frer seqRcvyAddEntry 0 -seqRcvyEN 1 -seqRcvyPort 7 -seqRcvyIndex 0xf -seqRcvyAlg 1
	sprintf(msgBuf, "%s %s %d %s %d %s %d %s %d %s %d",
		FIR_CMD_FRER, FIR_SEQ_RCVY_ADD_ENTRY, seqRcvyId, 
		FIR_OPTION_SEQ_RCVY_EN, seqRcvyEn,
		FIR_OPTION_SEQ_RCVY_PORT, seqRcvyPort,
		FIR_OPTION_SEQ_RCVY_INDEX, seqRcvyIndex,
		FIR_OPTION_SEQ_RCVY_ALG, seqRcvyAlg); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(msgBuf)
	{
		free(msgBuf);
	}
	
	return (rc);
}


/* frer bank1SCtrFlushAll */
/* frer bank1SCtrLoad 15 -bk1SCtrEn 1 -bk1SCtrPort 7 -bk1SCtrIndex 0xF */
int fir_cfg_b1SCtr(U8 sCtrId, U8 sCtrEn, U8 sCtrPort, U8 sCtrIndex)
{
	int rc = SOCKET_ERROR;
	char *msgBuf;

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		return SOCKET_ERROR;
	}

	msgBuf = malloc(FIR_CMD_MAX_SIZE);
	if (NULL == msgBuf) 
	{
		printf("malloc failed\n");
		return SOCKET_ERROR;
	}

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// frer bank1SCtrFlushAll
	sprintf(msgBuf, "%s %s", FIR_CMD_FRER, FIR_BANK1_SCTR_FLUSH_ALL); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(rc != SOCKET_OK)
	{
		if(msgBuf)
		{
			free(msgBuf);
		}	
		return rc;
	}   

	memset(msgBuf, 0, FIR_CMD_MAX_SIZE);
	// frer bank1SCtrLoad 15 -bk1SCtrEn 1 -bk1SCtrPort 7 -bk1SCtrIndex 0xF
	sprintf(msgBuf, "%s %s %d %s %d %s %d %s %d",
		FIR_CMD_FRER, FIR_BANK1_SCTR_LOAD, sCtrId,
		FIR_OPTION_BANK1_SCTR_EN, sCtrEn,
		FIR_OPTION_BANK1_SCTR_PORT, sCtrPort,
		FIR_OPTION_BANK1_SCTR_INDEX, sCtrIndex); 

	rc = fir_send_data(msgBuf, strlen(msgBuf));
	if(rc == strlen(msgBuf))
	{
		rc = SOCKET_OK;
	}
	else
	{
		rc = SOCKET_ERROR;
		printf("fir_send_data(%s) failed\n", msgBuf);
	}

	if(msgBuf)
	{
		free(msgBuf);
	}
	
	return (rc);
}


#ifdef SLEEP_BTW_CMD
void fir_sleep(void)
{
	U8 us_sleep = TIME_BTW_CMD;
	printf("usleeping... : %dus\n", us_sleep);
	usleep(us_sleep);
	return;
}
#endif


int fir_cfg_srp(char *macAddr, U16 qPri1Rate)
{
	int rc = SOCKET_ERROR;

	U8 fid = 1;
	U8 vid = 1; 

	U8 portTag[FIR_MAX_PORTS];

	U8 flush_all = 1;

	U8 entryState = 0xB;
	U16 portVec = 1 << fir_port_right;
	U8 macQPri = 0x2;

	U8 qMode = 0x2; // MSD_8021Q_CHECK

	U8 portSchedulePointer = 0x0; //Table 128
	U8 portScheduleMode = 0x3; // MSD_PORT_SCHED_WRR_PRI4_3_2_1_0

	U8 tsnPort;
	U8 regOffset;
	U16 regData;

	printf("fir_cfg_srp(%s, %ld) : from port #%d to port #%d\n", macAddr, qPri1Rate, fir_port_left, fir_port_right); 

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		return SOCKET_ERROR;
	}

#if 1
	for(U8 portNum = 0; portNum < FIR_MAX_PORTS; portNum++)
	{
		if((portNum == fir_port_left) || (portNum == fir_port_right))
		{
			portTag[portNum] = 2;
		}
		else
		{
			portTag[portNum] = 3;
		}
	}

	rc = fir_cfg_vtu(fid, vid, portTag);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_vtu() failed\n");
		goto err;
	}
	
	rc = fir_cfg_atu(flush_all, macAddr, entryState, fid, portVec, macQPri);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_atu() failed\n");
		goto err;
	}
 
	rc = fir_cfg_portQMode(fir_port_left, qMode);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_portQMode() for %d failed\n", fir_port_left);
		goto err;
	} 

	rc = fir_cfg_portQMode(fir_port_right, qMode);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_portQMode() for %d failed\n", fir_port_right);
		goto err;
	}  

	/* qbv write 0x1 0x1f 0x0 0x3322 */ 
	tsnPort = 0x1F; regOffset = 0x0; regData = 0x3322;
	if(fir_cfg_avbPolicy(tsnPort, regOffset, regData) == SOCKET_ERROR)
	{
		printf("fir_cfg_avbPolicy() for 0x%x, 0x%x failed\n", tsnPort, regOffset);
		goto err;
	} 

	/* qbv write 0x1 0x1f 0x4 0x0000 */ 
	tsnPort = 0x1F; regOffset = 0x4; regData = 0x0000;
	if(fir_cfg_avbPolicy(tsnPort, regOffset, regData) == SOCKET_ERROR)
	{
		printf("fir_cfg_avbPolicy() for 0x%x, 0x%x failed\n", tsnPort, regOffset);
		goto err;
	}  

	/* qbv write 0x1 0x2 0x0 0x8000 */
	regOffset = 0x0; regData = 0x8000;
	if(fir_cfg_avbPolicy(fir_port_left, regOffset, regData) == SOCKET_ERROR)
	{
		printf("fir_cfg_avbPolicy() for 0x%x, 0x%x failed\n", tsnPort, regOffset);
		goto err;
	}  

	// qbv write 0x2 0x1f 0x1 0x0064
	tsnPort = 0x1F; regOffset = 0x1; regData = 0x0064;
	if(fir_cfg_qav(tsnPort, regOffset, regData) == SOCKET_ERROR)
	{
		printf("fir_cfg_qav() for %d failed\n", fir_port_right);
		goto err;
	} 

	// qbv write 0x2 0x1f 0x0 0x8064
	tsnPort = 0x1F; regOffset = 0x0; regData = 0x8064;
	if(fir_cfg_qav(tsnPort, regOffset, regData) == SOCKET_ERROR)
	{
		printf("fir_cfg_qav() for %d failed\n", fir_port_right);
		goto err;
	}  

	// qbv write 0x2 0x6 0x4 0x0509
	tsnPort = fir_port_right; regOffset = 0x4; regData = qPri1Rate; // 0x0509;
	if(fir_cfg_qav(tsnPort, regOffset, regData) == SOCKET_ERROR)
	{
		printf("fir_cfg_qav() for %d failed\n", fir_port_right);
		goto err;
	}  
#endif

	rc = fir_cfg_portQCtrl(fir_port_right, portSchedulePointer, portScheduleMode);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_portQCtrl() for %d failed\n", fir_port_right);
		goto err;
	}  

err:
	return (rc);
}


int fir_cfg_frer_clear(void)
{
	printf("TBD : fir_cfg_frer_clear()\n");

	return SOCKET_OK;
}


int fir_cfg_frer_rep(char *macAddr)
{
	int rc = SOCKET_ERROR;

	U8 fid = 2;
	U8 vid = 5; 

	U8 portTag[FIR_MAX_PORTS];

	U8 qMode = 0x2; // MSD_8021Q_CHECK

	U8 tcamMode = 0x1; // MSD_TCAM_MODE_ENABLE_48

	U8 tcamEntryNum = 0;
	
	U16 spv = 1 << fir_port_left; // source port vector
	U16 spvMask = 0x3FF;

	U8 frameOctet[48] = {0x0};
	U8 frameOctetMask[48] = {0x0};
	frameOctet[1] = 0x22; // Match DA
	frameOctetMask[1] = 0xFF;

	U8 dpvMode = 0x3; // DPV Mode, Destination Port Vector Mode
	U32 dpvData = 0x48; // DPV/RPV Data, DPV or RPV Data

	U8 seqGenEn = 0x1; //enable sequence generation
	U8 indvRcvyId = 0xF; // use sequence generation instance id

	U8 flush_all = 1;

	U8 entryState = 0xE;
	U16 portVec = 1 << fir_port_left;
	U8 macQPri = 0x0; // TBD	

	printf("fir_cfg_frer_rep(%s) : from port #%d to port #%d and #%d\n", macAddr, fir_port_left, fir_port_right, fir_port_bottom);

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		goto err;
	}

	for(U8 portNum = 0; portNum < FIR_MAX_PORTS; portNum++)
	{
		if((portNum == fir_port_left) || (portNum == fir_port_right) || (portNum == fir_port_bottom))
		{
			portTag[portNum] = 2;
		}
		else
		{
			portTag[portNum] = 3;
		}
	}	

	rc = fir_cfg_vtu(fid, vid, portTag);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_vtu() failed\n");
		goto err;
	}
	
	rc = fir_cfg_portQMode(fir_port_left, qMode);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_portQMode() for %d failed\n", fir_port_left);
		goto err;
	} 

	rc = fir_cfg_portQMode(fir_port_right, qMode);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_portQMode() for %d failed\n", fir_port_right);
		goto err;
	}  

	rc = fir_cfg_portQMode(fir_port_bottom, qMode);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_portQMode() for %d failed\n", fir_port_bottom);
		goto err;
	}   

	rc = fir_cfg_tcamMode(fir_port_left, tcamMode);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_portQMode() for %d failed\n", fir_port_bottom);
		goto err;
	}  

	rc = fir_cfg_tcam(tcamEntryNum, 
		spv, spvMask, 
		frameOctet, frameOctetMask, 
		dpvMode, dpvData, 
		seqGenEn, indvRcvyId, 
		0x0, 0x0, /* frerSctrInc, frerSctrIndex : Elimination */
		0x0, 0x0, 0x0); /* frerSeqEncType, frerSeqRcvyEn, frerSeqRcvyIndex : Elimination */
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_tcam() failed\n");
		goto err;
	} 

	rc = fir_cfg_atu(flush_all, macAddr, entryState, fid, portVec, macQPri); // macQPri setting?
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_atu() failed\n");
		goto err;
	} 

err:
	return (rc);
}


int fir_cfg_frer_eli(char *macAddr)
{
	int rc = SOCKET_ERROR;

	U8 portMap[FIR_MAX_PORTS-2];

	U8 tcamMode = 0x1; // MSD_TCAM_MODE_ENABLE_48

	U8 portTag[FIR_MAX_PORTS];

	U8 fid = 2;
	U8 vid = 5; 

	U8 tcamEntryNum = 0;
	
	U16 spv = 0;
	U16 spvMask = 0x3FF ^ (0x1 << fir_port_left) ^ (0x1 << fir_port_bottom);

	U8 frameOctet[48] = {0x0};
	U8 frameOctetMask[48] = {0x0};
	frameOctet[1] = 0x22; // Match DA
	frameOctetMask[1] = 0xFF;

	U8 frerSctrInc = 0x1; // Increment the FRER Stream Counter
	U8 frerSctrIndex = 0xF; // Stream Counter Index == indvRcvyId ??
	U8 frerSeqEncType = 0x1; // Sequence Encapsulate Type

	U8 frerSeqRcvyEn = 0x1;
	U8 frerSeqRcvyIndex = 0xF;

	U8 seqRcvyId = 0x0; // FrerSeq_RcvyID, Sequence Recovery Instanace Identifier
	U8 seqRcvyEn = 0x1; // SeqRcvyEn, Sequence Recovery Enable
	U8 seqRcvyPort = fir_port_right; // SeqRcvy_Port, Sequence Recovery Port
	U8 seqRcvyIndex = 0xF; //SeqRcvy_Index, Sequence Recovery Index
	U8 seqRcvyAlg = 0x1; // SeqRcvy_Alg, Sequence Recovery Algorithm

	U8 sCtrId = 0xF; //B1SCtrID, Bank 1 Stream Counter Instance Identifier
	U8 sCtrEn = 0x1; // B1SCtrEn, Bank 1 Stream Counter Enable
	U8 sCtrPort = fir_port_right; // B1SCtrPort, Bank 1 Stream Counter Port
	U8 sCtrIndex = 0xF; //FrerB1SCtr_Index, Bank 1 Stream Counter Index

	printf("fir_cfg_frer_eli(%s) : from port #%d and #%d to port #%d\n", macAddr, fir_port_left, fir_port_bottom, fir_port_right); 

	if(fir_sock == SOCKET_ERROR) 
	{
		printf("fir_sock is SOCKET_ERROR\n");
		goto err;
	}

	U8 portNum, idx;
	for(portNum = 0, idx = 0; portNum < FIR_MAX_PORTS; portNum++)
	{
		if((portNum != fir_port_left) && (portNum != fir_port_bottom))
		{
			portMap[idx++] = portNum;
		}
	}	 

	rc = fir_cfg_portVlanMap(fir_port_left, portMap);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_portVlanMap() for %d failed\n", fir_port_left);
		goto err;
	}

	rc = fir_cfg_portVlanMap(fir_port_bottom, portMap);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_portVlanMap() for %d failed\n", fir_port_bottom);
		goto err;
	} 

	sleep(1);

	rc = fir_cfg_tcamMode(fir_port_left, tcamMode);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_tcamMode() for %d failed\n", fir_port_left);
		goto err;
	}  

	sleep(1);

	rc = fir_cfg_tcamMode(fir_port_bottom, tcamMode);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_tcamMode() for %d failed\n", fir_port_bottom);
		goto err;
	}   

	sleep(1);

	for(U8 portNum = 0; portNum < FIR_MAX_PORTS; portNum++)
	{
		if((portNum == fir_port_left) || (portNum == fir_port_bottom) || (portNum == fir_port_right))
		{
			portTag[portNum] = 2;
		}
		else
		{
			portTag[portNum] = 3;
		}
	}	

	rc = fir_cfg_vtu(fid, vid, portTag);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_vtu() failed\n");
		goto err;
	}
	
	/* tcam delAll ?? */

	/* tcam addEntry -frerSctrInc 1 -frerSctrIndex 0xF -frerSeqEncType 1 -frerSeqRcvyEn 1 -frerSeqRcvyIndex 0xF */
	rc = fir_cfg_tcam(tcamEntryNum, 
		spv, spvMask, 
		frameOctet, frameOctetMask, 
		0x0, 0x0, /* dpvMode, dpvData : Replication */
		0x0, 0x0, /* frerSeqGenEn, frerIndvRcvyId : Replication */
		frerSctrInc, frerSctrIndex, 
		frerSeqEncType, frerSeqRcvyEn, frerSeqRcvyIndex);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_tcam() failed\n");
		goto err;
	} 

	// frer seqRcvyFlushAll
	// frer seqRcvyReadClear 0 : 0 = SeqRcvyId
	// frer seqRcvyAddEntry 0 -seqRcvyEN 1 -seqRcvyPort 7 -seqRcvyIndex 0xf -seqRcvyAlg 1
	rc = fir_cfg_seqRcvy(seqRcvyId, seqRcvyEn, seqRcvyPort, seqRcvyIndex, seqRcvyAlg);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_seqRcvy() failed\n");
		goto err;
	}  

	// frer bank1SCtrFlushAll
	// frer bank1SCtrLoad 15 -bk1SCtrEn 1 -bk1SCtrPort 7 -bk1SCtrIndex 0xF
	rc = fir_cfg_b1SCtr(sCtrId, sCtrEn, sCtrPort, sCtrIndex);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_b1StrC() failed\n");
		goto err;
	} 

#if 0
	rc = fir_cfg_portQMode(ingress, qMode);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_portQMode() for %d failed\n", ingress);
		goto err;
	} 

	rc = fir_cfg_portQMode(egress1, qMode);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_portQMode() for %d failed\n", egress1);
		goto err;
	}  

	rc = fir_cfg_portQMode(egress2, qMode);
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_portQMode() for %d failed\n", egress2);
		goto err;
	}   

	rc = fir_cfg_atu(flush_all, macAddr, entryState, fid, portVec, macQPri); // macQPri setting?
	if(rc == SOCKET_ERROR)
	{
		printf("fir_cfg_atu() failed\n");
		goto err;
	} 
#endif

err:
	return (rc);
}


int fir_cfg_frer(unsigned frer_fn)
{
	int err = 0;
	
	char *macAddr = "002222222222"; 

	printf("fir_cfg_frer() : frer_fn = %d\n", frer_fn);

	if(frer_fn == FIR_FRER_NONE)
	{
		fir_cfg_frer_clear();
	}
	else if(frer_fn == FIR_FRER_REPLICATION)
	{
		err = fir_cfg_frer_rep(macAddr);
		if(err != SOCKET_OK)
		{
			printf("fir_cfg_frer_rep() failed\n");
			return 0;
		}  
	}
	else if(frer_fn == FIR_FRER_ELIMINATION)
	{
		err = fir_cfg_frer_eli(macAddr);
		if(err != SOCKET_OK)
		{
			printf("fir_cfg_frer_eli() failed\n");
			return 0;
		} 
	}
	else
	{
		printf("Unknown FRER function : %d\n", frer_fn);
		return 0; /* FALSE */
	}

	return 1; /* TRUE */
}


int openavbFirInitialize(unsigned port_left, unsigned port_right, unsigned port_bottom, unsigned frer_fn)
{
	fir_port_left = port_left;
	fir_port_right = port_right;
	fir_port_bottom = port_bottom;

	fir_connect();

	if(!fir_cfg_frer(frer_fn))
	{
		printf("fir_cfg_frer() failed : frer_fn=%d\n", frer_fn);
		return 0; /* FALSE */
	}

	return 1; /* TRUE */
}


void openavbFirFinalize(void)
{
	fir_disconnect();
}


int main(int argc, char *argv[]) 
{
	int rc = SOCKET_ERROR;

	fir_sock = fir_connect();
	if(fir_sock != SOCKET_ERROR) 
	{
		rc = fir_send_data("port getQMode 2", sizeof("port getQMode 2"));
		if(rc == SOCKET_ERROR)
		{
			printf("fir_send_data() failed\n");
		}
	}
	else
	{
		printf("fir_connect() failed\n");
	}


	return 0;
}
