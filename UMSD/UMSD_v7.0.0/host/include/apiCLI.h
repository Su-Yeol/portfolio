/*******************************************************************************
*
* apiCLI.h
*
* DESCRIPTION:
*       Command Line Interface
*
* DEPENDENCIES:   Platform
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef apiCLI_h
#define apiCLI_h

#ifdef __cplusplus
extern "C" {
#endif

#include "libcli.h"
#include "cJSON.h"

typedef struct CLI_COMMAND_STRUCT_
{
    char *cmd;
    int(*callback)(void);
}CLI_COMMAND_STRUCT;

typedef struct EXTERN_CLI_STRUCT_
{
	char *cmd;
	CLI_COMMAND_STRUCT *cmdList;
}EXTERN_CLI_STRUCT;

typedef MSD_STATUS(*API_SET_PORT_U32) (MSD_U8, MSD_LPORT, MSD_U32);
typedef MSD_STATUS(*API_GET_PORT_PU32) (MSD_U8, MSD_LPORT, MSD_U32*);

typedef MSD_STATUS(*API_SET_PORT_BOOL_U32) (MSD_U8, MSD_LPORT, MSD_BOOL, MSD_U32);
typedef MSD_STATUS(*API_GET_PORT_PBOOL_PU32) (MSD_U8, MSD_LPORT, MSD_BOOL*, MSD_U32*);

typedef MSD_STATUS(*API_SET_PORT_U32_U32) (MSD_U8, MSD_LPORT, MSD_U32, MSD_U32);
typedef MSD_STATUS(*API_SET_PORT_U32_U64) (MSD_U8, MSD_LPORT, MSD_U32, MSD_U64);
typedef MSD_STATUS(*API_GET_PORT_U32_PU32) (MSD_U8, MSD_LPORT, MSD_U32, MSD_U32*);
typedef MSD_STATUS(*API_GET_PORT_PU32_PU32) (MSD_U8, MSD_LPORT, MSD_U32*, MSD_U32*);
typedef MSD_STATUS(*API_SET_PORT_U32_U32_U32) (MSD_U8, MSD_LPORT, MSD_U32, MSD_U32, MSD_U32);
typedef MSD_STATUS(*API_GET_PORT_U32__U32_PU32) (MSD_U8, MSD_LPORT, MSD_U32, MSD_U32, MSD_U32*);

typedef MSD_STATUS(*API_SET_PORT_U32_U32_U32_U32) (MSD_U8, MSD_LPORT, MSD_U32, MSD_U32, MSD_U32, MSD_U32);
typedef MSD_STATUS(*API_GET_PORT_U32__U32_U32_PU32) (MSD_U8, MSD_LPORT, MSD_U32, MSD_U32, MSD_U32, MSD_U32*);

typedef MSD_STATUS(*API_SET_U32) (MSD_U8, MSD_U32);
typedef MSD_STATUS(*API_GET_PU32) (MSD_U8, MSD_U32*);

typedef MSD_STATUS(*API_SET_U32_U32) (MSD_U8, MSD_U32, MSD_U32);
typedef MSD_STATUS(*API_GET_U32_PU32) (MSD_U8, MSD_U32, MSD_U32*);

typedef MSD_STATUS(*API_SET_DEV) (MSD_U8);

typedef int(*CALLBACE)(void);

typedef void(*PRINT_CALLBACK)(const char *string);
void setPrintCallback(PRINT_CALLBACK callback);

typedef struct CLI_PAIR_API_STRUCT_
{
    char* setCmd;
    void* setFunc;

    char* getCmd;
    void* getFunc;
}CLI_PAIR_API_STRUCT;


/* root/API JSON*/
cJSON *rootJSON;
cJSON *rootAPIJSON;

cJSON *parseJSONfile(void);
cJSON *parseAPIJSONfile(void);


/* static function*/
static char* stristr(const char* str1, const char* str2);
static MSD_U16 ReadInt(MSD_U8 *data, MSD_U8 cnt);
static int UpdateFilename(char *tempName);
static void displayParaHelp(char* cmd, char* subcmd);
static char* reflectMSD_STATUS(MSD_STATUS status);
static int updateArray(int* index, char* value, int maxLen, unsigned int list[]);

static void displayTcamEgrData(MSD_TCAM_EGR_DATA *tcamEgrData);
static void displayTcamEgrData(MSD_TCAM_EGR_DATA *tcamEgrData);
static int updateFdbEntry(MSD_ATU_ENTRY * entry, int startIndex);
static void printOutFdbEntry(MSD_ATU_ENTRY * entry);
static void printOutStuEntry(MSD_STU_ENTRY *entry);
static void printOutVtuEntry(MSD_VTU_ENTRY *entry);
static void printOutPtpPortConfig(MSD_PTP_PORT_CONFIG *ptpData);
static void printOutPtpGlobalConfig(MSD_PTP_GLOBAL_CONFIG *ptpData);
static void displayCounter(MSD_STATS_COUNTER_SET *statsCounter, MSD_BOOL fullbank);
static int updateMemPortsArray(MSD_LPORT* memPorts, int len, int startIndex);
static int updateStuEntry(MSD_STU_ENTRY *entry, int startIndex);
static void printOutWeight(MSD_QoS_WEIGHT* weight);
static void printOutPtpIntStatus(MSD_PTP_INTERRUPT_STATUS *ptpIntSt);
static int updateIPV4Header(MSD_TCAM_ENT_IPV4_UNT *ipv4, MSD_TCAM_ENT_IPV4_UNT *ipv4Mask, char* cmd, char* value);
static int updateIPV6Header(MSD_TCAM_ENT_IPV6_UNT *ipv6, MSD_TCAM_ENT_IPV6_UNT *ipv6Mask, char* cmd, char* value);
static int updateUDPHeader(MSD_TCAM_ENT_UDP_UNT *udp, MSD_TCAM_ENT_UDP_UNT *udpMask, char* cmd, char* value);
static int updateTCPHeader(MSD_TCAM_ENT_TCP_UNT *tcp, MSD_TCAM_ENT_TCP_UNT *tcpMask, char* cmd, char* value);
static void v4ipStr2ipAddr(char* str, MSD_U8* addr);
static void printOutIrlRes(MSD_PIRL_DATA *pirlData);
static int updatePtpMode(MSD_PTP_MODE_CONFIG *mode, int startIndex);
static int updateAdvConfigParameter(MSD_TCAM_ENT_KEY *keyMaskPtr, MSD_TCAM_ENT_KEY *keyPtr, MSD_TCAM_ENT_ACT *action, MSD_TCAM_PKT_TYPE pktType, MSD_TCAM_ENT_PKT_UNT *patternPtr, MSD_TCAM_ENT_PKT_UNT *maskPtr, int startIndex);
static void displayAdvVCTResult(MSD_ADV_CABLE_STATUS *cableStatus, int channel, MSD_ADV_VCT_MODE mode);
static int updateVctMode(MSD_ADV_VCT_MODE *mode, int startIndex);
static void macStr2macArr(char* inp, MSD_U8* out);
static void printOutCutThrough(MSD_LPORT port, MSD_CUT_THROUGH* cutThru);
static void printOutCutThrough(MSD_LPORT port, MSD_CUT_THROUGH* cutThru);
static int updateCustomStruct(MSD_PIRL_CUSTOM_RATE_LIMIT *customSetup, int startIndex);
static void displayTcamRCCompData(MSD_TCAM_RC_COMP_DATA *tcamrccompData);
static void displayTcamData(MSD_TCAM_DATA *tcamData);
static int updatePortConfig(MSD_PTP_PORT_CONFIG* ptpData, int startIndex);
static void printOutTimeStamp(MSD_PTP_TS_STATUS *ptpStatus);
static int parseTxSecY(MSD_SECY_EGRESS* secY, int startIndex);
static int parseRxSecY(MSD_SECY_INGRESS* secY, int startIndex);
static int parseRxSC(MSD_MSEC_RX_SC* rxSC, int startIndex);
static int parseRxSA(MSD_MSEC_RX_SA* rxSA, int startIndex);
static int parseTxSC(MSD_MSEC_TX_SC* txSC, int startIndex);
static int parseTxSA(MSD_MSEC_TX_SA* txSA, int startIndex);
static int displayTxSecY(MSD_SECY_EGRESS* secY);
static int displayRxSecY(MSD_SECY_INGRESS* secY);
static int displayRule(MSD_MSEC_RULE* rule);
static int displayRxSC(MSD_MSEC_RX_SC* rxSC);
static int displayRxSA(MSD_MSEC_RX_SA* rxSA);
static int displayRxSANextPN(MSD_U64 nextPN);
static int displayTxSC(MSD_MSEC_TX_SC* txSC);
static int displayTxSA(MSD_MSEC_TX_SA* txSA);
static int displayTxSANextPN(MSD_U64 nextPN);
static int displayRXSCStates(MSD_MSEC_RX_SC_COUNTER* states);
static int displayRXSecYStates(MSD_MSEC_RX_SECY_COUNTER* states);
static int displayTXSCStates(MSD_MSEC_TX_SC_COUNTER* states);
static int displayTXSecYStates(MSD_MSEC_TX_SECY_COUNTER* states);
static int displayAllRXSCStates(MSD_MSEC_RX_SC_COUNTER rxSCStates[]);
static int displayAllRXSecYStates(MSD_MSEC_RX_SECY_COUNTER rxSecYStates[]);
static int displayAllTXSCStates(MSD_MSEC_TX_SC_COUNTER txSCStates[]);
static int displayAllTXSecYStates(MSD_MSEC_TX_SECY_COUNTER txSecYStates[]);
static int displayTxPNThreshold(MSD_U32 pNThr);
static int displayTxXPNThreshold(MSD_U64 pNThr);
static int displayRxDefaultSCI(MSD_U64 defaultSCI);
static int displayNextInterruptStatus1(MSD_U8 nextIndex, MSD_MACSEC_INT_TXSC* interrupt);
int strIcmp(const char* p1, const char* p2);
int UnitTestCases(void);
int SampleCases(void);
int CustomizedCases(void);

int fileOperation(void);
int paraHelp(void);

int atuAddEntry(void);
int atuGetNextEntry(void);
int atuFlush(void);
int atuFlushInDB(void);
int atuMove(void);
int atuMoveInDB(void);
int atuDelEntry(void);
int atuGetVio(void);
int atuFindEntry(void);
int atuPortRemove(void);
int portRemoveINDB(void);

int ecidAddEntry(void);
int ecidDelAll(void);
int ecidGetNextEntry(void);
int ecidFindEntry(void);
int ecidGetCount(void);

int stuAddEntry(void);
int stuGetNext(void);
int stuFindEntry(void);
int stuDelEntry(void);
int stuGetEntryCount(void);

int vtuAddEntry(void);
int vtuFlush(void);
int vtuFindEntry(void);
int vtuDelEntry(void);
int vtuSetMode(void);
int vtuGetMode(void);
int vtuGetVio(void);
int vutIsExist(void);
int vtuGetNext(void);
int vtuGetCount(void);
int vtuSetMemberTag(void);
int vtuGetMemberTag(void);

int IMPRun(void);
int IMPReset(void);
int IMPStop(void);
int IMPWriteEEPROM(void);
int IMPLoadToEEPROM(void);
int IMPLoadToRAM(void);

int phyReset(void);
int phyHelp(void);
int phyOperation(void);
int phyGetIntStatus(void);
int devGetIntStatus(void);
int activeGetIntStatus(void);

int irlInit(void);
int irlInitRes(void);
int irlCustomSetup(void);
int irlReadRes(void);
int irlWriteRes(void);
int irlRReg(void);
int irlWReg(void);
int irlAdvConfig(void);
int irlAdvConfigDisable(void);

int vctCableDiag(void);
int vctExtenedStatus(void);

int qavSetXRate(void);
int qavGetXRate(void);
int qavSetXHiLimit(void);
int qavGetXHiLimit(void);

int ptpSetIntEn(void);
int ptpGetIntEn(void);
int ptpSetPortConfig(void);
int ptpGetPortConfig(void);
int ptpGetTimeStamp(void);
int ptpSetGlobalConfig(void);
int ptpGetGlobalConfig(void);
int ptpGetIntStatus(void);
int ptpSetPtpMode(void);
int ptpGetPtpMode(void);
int ptpStoreCompensation(void);
int ptpStoreAll(void);
int ptpCaptureAll(void);
int ptpSetPtpPulse(void);
int ptpGetPtpPulse(void);

int qosSetPrioMap(void);
int qosGetPrioMap(void);

int tcamFlushAll(void);
int tcamFlushEntry(void);
int tcamAddEntry(void);
int tcamModifyEntry(void);
int tcamCopyEntry(void);
int tcamReadEntry(void);
int tcamGetNextEntry(void);
int tcamFindEntry(void);
int tcamSetMode(void);
int tcamGetMode(void);
int tcamAddPort(void);
int tcamRemovePort(void);
int tcamEgrDelAllPorts(void);
int tcamEgrDelEntry(void);
int tcamEgrAddEntry(void);
int tcamEgrReadEntry(void);
int tcamEgrGetNextEntry(void);
int tcamAdvConfig(void);

int tcamrcFlushAll(void);
int tcamrcFlushEntry(void);
int tcamrcAddEntry(void);
int tcamrcReadEntry(void);
int tcamrcGetNextEntry(void);
int tcamrcFindEntry(void);

int tcamrccompCompAdd(void);
int tcamrccompCompRead(void);
int tcamrccompFlushAll(void);
int tcamrccompFlushPort(void);

int arpFlushAll(void);
int arpFlushEntry(void);
int arpAddMcEntry(void);
int arpAddUcEntry(void);
int arpReadEntry(void);
int arpGetNextEntry(void);

int rmuGetID(void);
int rmuDumpAtu(void);
int rmuDumpEcid(void);
int rmuDumpMib2(void);
int rmuDumpTcam(void);
int rmuDumpMib(void);
int rmuTcamRead(void);
int rmuTcamWrite(void);
int rmuDumpReg(void);

int portGetVlanPorts(void);
int portSetVlanPorts(void);
int portSetCutThrough(void);
int portGetCutThrough(void);
int portGetPortStatus(void);

int mibFlushAll(void);
int mibFlushPort(void);
int mibGetPortCounter(void);
int mibGetPortAllCounters(void);
int mibDump(void);

int qcSetQosWeight(void);
int qcGetQosWeight(void);
int qcSetPortQosWeight(void);
int qcGetPortQosWeight(void);

int ReadRegister(void);
int WriteRegister(void);

int debugLvlSet(void);

/*
int ReadRegisterField(void);
int WriteRegisterField(void);

int setBusInterface(void);
*/
int getBusInterface(void);
int getVersion(void);

int apiCLIHelp(void);
int execSystemCmd(void);
int exitProgram(void);
int parseCmd(char *cmd);
void apiCLI_alloc_buf(void);
void apiCLI_free_buf(void);
int parseCmd_cli(const char *command, char *argv[], int argc);
int cmd_operation(struct cli_def *cli, const char *command, char *argv[], int argc);
int cmd_register_callback(cJSON *temp1, cJSON *temp2);
int cmd_direct_register_callback(cJSON *temp);

int qcrSetGrpId(void);
int qcrGetGrpId(void);
int qcrSetCbsLimit(void);
int qcrGetCbsLimit(void);
int qcrSetRateLimit(void);
int qcrGetRateLimit(void);
int qcrSetMaxResidenceTime(void);
int qcrGetMaxResidenceTime(void);
int qcrFlushAll(void);
int qcrFlushOne(void);

int frerSeqGenFlushAll(void);
int frerSeqGenFlushOne(void);
int frerSeqGenLoad(void);
int frerSeqGenRead(void);

int frerIndvRcvyFlushAll(void);
int frerIndvRcvyFlushOne(void);
int frerIndvRcvyLoad(void);
int frerIndvRcvyRead(void);
int frerIndvRcvyGetNext(void);
int frerIndvRcvyReadClear(void);

int frerSeqRcvyFlushAll(void);
int frerSeqRcvyFlushOne(void);
int frerSeqRcvyLoad(void);
int frerSeqRcvyRead(void);
int frerSeqRcvyGetNext(void);
int frerSeqRcvyGetViolation(void);
int frerSeqRcvyReadClear(void);

int frerBank0SCtrFlushAll(void);
int frerBank0SCtrFlushCtr(void);
int frerBank0SCtrLoad(void);
int frerBank0SCtrRead(void);
int frerBank0SCtrGetNext(void);
int frerBank0SCtrReadClear(void);

int frerBank1SCtrFlushAll(void);
int frerBank1SCtrFlushCtr(void);
int frerBank1SCtrLoad(void);
int frerBank1SCtrRead(void);
int frerBank1SCtrGetNext(void);
int frerBank1SCtrReadClear(void);

int qciStreamFilterFlushAll(void);
int qciStreamGateFlushAll(void);
int qciStreamGateAddEntry(void);
int qciStreamGateTimeGenFlushAll(void);
int qciFlowMeterFlushAll(void);

int macsecConfigureEgress(void);
int macsecConfigureIngress(void);
int macsecFlushAll(void);
int macsecCreateRule(void);
int macsecDeleteRule(void);
int macsecGetRule(void);
int macsecEnableIngressSet(void);
int macsecEnableIngressGet(void);
int macsecEnableEgressSet(void);
int macsecEnableEgressGet(void);
int macsecCreateTxSecY(void);
int macsecDeleteTxSecY(void);
int macsecGetTxSecY(void);
int macsecCreateRxSecY(void);
int macsecDeleteRxSecY(void);
int macsecGetRxSecY(void);
int macsecCreateRxSC(void);
int macsecDeleteRxSC(void);
int macsecGetRxSC(void);
int macsecCreateRxSA(void);
int macsecDeleteRxSA(void);
int macsecGetRxSA(void);
int macsecGetRxSANextPN(void);
int macsecCreateTxSC(void);
int macsecDeleteTxSC(void);
int macsecGetTxSC(void);
int macsecCreateTxSA(void);
int macsecDeleteTxSA(void);
int macsecGetTxSA(void);
int macsecGetTxSANextPN(void);
int macsecGetRXSCStates(void);
int macsecGetRXSecYStates(void);
int macsecGetTXSCStates(void);
int macsecGetTXSecYStates(void);
int macsecSetTxPNThreshold(void);
int macsecSetTxXPNThreshold(void);
int macsecGetTxPNThreshold(void);
int macsecGetTxXPNThreshold(void);
int macsecSetRxDefaultSCI(void);
int macsecGetRxDefaultSCI(void);
int macsecSetTxSCIntCtrl(void);
int macsecGetTxSCIntCtrl(void);
int macsecGetNextTxSCIntStatus(void);

/* Enable hardware semaphore */
int enableHwSemaphore(void);
/* Disable hardware semaphore */
int disableHwSemaphore(void);

static int fullChipReset(void);
static int setPortU32U32(void);
static int setPortU32U64(void);
static int getPortU32PU32(void);
static int setPortU32U32U32(void);
static int getPortU32U32PU32(void);
static int setPortU32U32U32U32(void);
static int getPortU32U32U32PU32(void);
static int setPortU32(void);
static int getPortPU32(void);
static int setU32(void);
static int getPU32(void);
static int setDev(void);
static int getPortPU32PU32(void);
static int setU32U32(void);
static int getU32PU32(void);

int umsdApiSearch(void);
int umsdApiMan(void);

#ifdef __cplusplus
}
#endif

#endif