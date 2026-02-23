// --------------------------------------------------------------------------
// |              _    _ _______     .----.      _____         _____        |
// |         /\  | |  | |__   __|  .  ____ .    / ____|  /\   |  __ \       |
// |        /  \ | |  | |  | |    .  / __ \ .  | (___   /  \  | |__) |      |
// |       / /\ \| |  | |  | |   .  / / / / v   \___ \ / /\ \ |  _  /       |
// |      / /__\ \ |__| |  | |   . / /_/ /  .   ____) / /__\ \| | \ \       |
// |     /________\____/   |_|   ^ \____/  .   |_____/________\_|  \_\      |
// |                              . _ _  .                                  |
// --------------------------------------------------------------------------
//
// All Rights Reserved.
// Any use of this source code is subject to a license agreement with the
// AUTOSAR development cooperation.
// More information is available at www.autosar.org.
//
// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

/* includes */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include <demoUtil.h>
#include <wrsomeip/someipLogger.h>
#include <wrsomeip/wrsomeip.h>

/* defines */

#define PAYLOAD_SIZE 10
#define PAYLOAD_SIZE_EXTEND 1024 /* extended packet size of SOME/IP-TP */
#define RESP_EXP "This is a response message"
#define PROTO_UDP 0
#define PROTO_TCP 1
#define DEAFULT_REL PROTO_UDP
#define PAYLOAD_RESP_SIZE 120

/* locals */

LOCAL BOOL appRunning = TRUE;
LOCAL BOOL available = FALSE;
LOCAL BOOL reliable = FALSE; /* communication type */
LOCAL uint32_t cycle = DEMO_CYCLE / 1000; /* cyclic request (s) */
LOCAL u_int8_t requests = 2;
LOCAL uint8_t pldRespExp[PAYLOAD_RESP_SIZE];
LOCAL uint8_t* pldData = NULL;
LOCAL char* proto[2] = {"UDP", "TCP"};
LOCAL int pktSz = PAYLOAD_SIZE;
LOCAL SD_SERVICE servId = DEMO_SERV;
LOCAL uint16_t servRange = DEMO_SERV_RANGE;
LOCAL SD_INSTANCE instId = DEMO_INST;
LOCAL uint16_t instRange = DEMO_INST_RANGE;
LOCAL METHOD methId = DEMO_METH;
LOCAL uint16_t methRange = DEMO_METH_RANGE;
LOCAL DEMO_TS* pAvailTsArr; /* availability timestamps array */
LOCAL DEMO_TS* pRespTsArr; /* response timestamps array */
LOCAL BOOL* pAvailArr; /* service availability array */
LOCAL BOOL* pRespRecv; /* response received array */

LOCAL int logFD = 0;

/* forward declarations */

LOCAL void onAvailability(SERVICE_APPLICATION* pSa, SERVICE service, INSTANCE instance, BOOL isAvail);
LOCAL void onMessage(SERVICE_APPLICATION* pSa, SOMEIP_MESSAGE* pMsgResp);
LOCAL void sendRequest(SERVICE_APPLICATION* pClApp, SERVICE serv, INSTANCE inst);
LOCAL void usage(char* argv[]);
LOCAL void doCleanup(SERVICE_APPLICATION* pSrvApp);

static uint64_t getTimeStamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * (uint64_t)1000000 + tv.tv_usec;
}

/*******************************************************************************
 *
 * onAvailability - service availability event
 *
 * This is routine is called by service application when service availability
 * changes.
 *
 * RETURNS: N/A
 */

LOCAL void onAvailability(SERVICE_APPLICATION* pSa, SERVICE service, INSTANCE instance, BOOL isAvail)
{
    int idx;
    int us = 0;

    (void)(pSa);

    SOMEIP_LOG(SOMEIP_LOG_INFO, "onAvailability is triggered !");

    if ((service >= servId + servRange) || (instance >= (instId + instRange))) {
        SOMEIP_LOG(
            SOMEIP_LOG_ERROR, "Service availability [0x%04X:0x%04X:%d] not requested", service, instance, isAvail);
        return;
    }

    idx = (service - servId) * instRange + (instance - instId);
    us = demoUtilTsDif(&pAvailTsArr[idx], NULL);
    pAvailArr[idx] = isAvail;

    SOMEIP_LOG(SOMEIP_LOG_INFO,
        "Service [0x%04X:0x%04X] isAvailable [%d] after [%dsec %dms "
        "%dus]",
        service,
        instance,
        isAvail,
        us / USEC_PER_SEC,
        (us / USEC_PER_MSEC) % USEC_PER_MSEC,
        us % USEC_PER_MSEC);

    if (isAvail) {
        available = TRUE;
        SOMEIP_LOG(SOMEIP_LOG_INFO, "Client just found a suitable find service!");

        SOMEIP_LOG(SOMEIP_LOG_INFO,
            "Trigger a send request message to the offering "
            "application!");
        // sendRequest (pSa, service, instance);
    }
}

/****************************************************************************
 *
 * onMessage - SOME/IP messages handler
 *
 * This routine is used to handle the received SOMEIP messages in the Client
 * side.
 *
 * RETURNS: N/A
 */

LOCAL void onMessage(SERVICE_APPLICATION* pSa, SOMEIP_MESSAGE* pMsgResp)
{
    SOMEIP_HEADER* pHdr;
    SOMEIP_PAYLOAD* pPld;
    int idx;
    int us;

    (void)pSa;

    SOMEIP_LOG(SOMEIP_LOG_INFO, "onMessage");

    if ((pMsgResp == NULL) || (pMsgResp->pHdr == NULL) || (pMsgResp->pPld == NULL)) {
        SOMEIP_LOG(SOMEIP_LOG_INFO, "onMessage null");
        return;
    }

    pHdr = pMsgResp->pHdr;
    pPld = pMsgResp->pPld;
    SOMEIP_LOG(SOMEIP_LOG_INFO,
        "Receive a response from service [0x%04X.0x%04X] "
        "client/session [0x%04X.0x%04X] size [%d]",
        pHdr->service,
        pHdr->instance,
        pHdr->client,
        pHdr->session,
        pPld->size);

    if ((pHdr->service >= (servId + servRange)) || (pHdr->instance >= (instId + instRange))) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR,
            "Response received for not requested method "
            "[0x%04X:0x%04X:0x%04X]",
            pHdr->service,
            pHdr->instance,
            pHdr->method);
        return;
    }

    idx = ((pHdr->method - methId) * servRange * instRange) + (pHdr->service - servId) * instRange
        + (pHdr->instance - instId);

    us = demoUtilTsDif(&pRespTsArr[idx], NULL);
    pRespRecv[idx] = TRUE;

    SOMEIP_LOG(SOMEIP_LOG_INFO,
        "Receive a response for method [0x%04X.0x%04X.0x%04X] latency: "
        "%dsec%dms%dus",
        pHdr->service,
        pHdr->instance,
        pHdr->method,
        us / USEC_PER_SEC,
        (us / USEC_PER_MSEC) % USEC_PER_MSEC,
        us % USEC_PER_MSEC);

    if (logFD) {
        // Log to file as workaround until DLT file logging is reliable
        char logmessage[1024];
        snprintf(logmessage,
            1024,
            "[%lu] [0x%04X.0x%04X.0x%04X] latency: "
            "%08dus\n",
            getTimeStamp(),
            pHdr->service,
            pHdr->instance,
            pHdr->method,
            us);

        ssize_t bytes = write(logFD, logmessage, strlen(logmessage));
        if (bytes != (ssize_t)strlen(logmessage)) {
            SOMEIP_LOG(SOMEIP_LOG_ERROR, "File to write to log file");
        }
    }

    if (memcmp(pPld->pData, pldRespExp, PAYLOAD_RESP_SIZE) == 0) {
        SOMEIP_LOG(SOMEIP_LOG_INFO, "PASS client side");
    } else {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "FAIL client side");
    }
}

/****************************************************************************
 *
 * sendRequest - send request message
 *
 * This is routine is used to send request message.
 *
 * RETURNS: N/A
 */

LOCAL void sendRequest(SERVICE_APPLICATION* pClApp, SERVICE serv, INSTANCE inst)
{
    SOMEIP_MESSAGE* pMsgReq;
    SOMEIP_PAYLOAD* pPld;
    STATUS rc;
    uint16_t i;
    int idxS;
    int idxM;

    SOMEIP_LOG(SOMEIP_LOG_INFO, "Requesting service [0x%04X.0x%04X]", serv, inst);

    pPld = someipPayloadNew(pldData, pktSz);
    if (pPld == NULL) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "Can not create request payload!");
        return;
    }

    pMsgReq = someipCreateRequest(reliable);
    if (pMsgReq == NULL) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "Can not create request message!");
        someipPayloadDelete(pPld);
        return;
    }

    pMsgReq->pPld = pPld;
    pMsgReq->pHdr->service = serv;
    pMsgReq->pHdr->instance = inst;

    idxS = (serv - servId) * instRange + (inst - instId);
    if (pAvailArr[idxS]) {
        /* handle all required method-ids */

        for (i = 0; i < methRange; i++) {
            pMsgReq->pHdr->method = methId + i;
            rc = servAppSend(pClApp, pMsgReq);
            if (rc != OK) {
                SOMEIP_LOG(SOMEIP_LOG_ERROR, "Can not send request message!");
            }
            idxM = (i * servRange * instRange) + (pMsgReq->pHdr->service - servId) * instRange
                + (pMsgReq->pHdr->instance - instId);
            pRespRecv[idxM] = FALSE;
            demoUtilTsSet(&pRespTsArr[idxM]);
        }
    }

    someipPayloadDelete(pPld);
    someipMessageDelete(pMsgReq, NOT_RCV_MSG);
}

/****************************************************************************
 *
 * usage - launch option helper
 *
 * This is routine is used to display usage helper of the application
 *
 * RETURNS: N/A
 */

LOCAL void usage(char* argv[])
{
    SOMEIP_LOG(SOMEIP_LOG_INFO, "Wrong arguments!");
    SOMEIP_LOG(SOMEIP_LOG_INFO,
        "usage: %s [-p <UDP|TCP>] [-s <service-id>] [-S <service-range]"
        "[-i <instance-id>] [-I <instance-range>] [-z <size>] "
        "[-c <cycle>] [-v]",
        argv[0]);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "OPTIONS:");
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -p <udp|tcp>, default %s", proto[DEAFULT_REL]);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -s <service-id> - service-id, default %d", servId);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -S <service-range> - service-id range, default %d", servRange);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -i <instance-id> - instance-id, default %d", instId);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -I <instance-range> - instance-id range, default %d", instRange);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -m <method-id> - method-id, default %d", methId);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -M <method-range> - method-id range, default %d", methRange);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -z <size>, default %d", pktSz);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -c <cycle>, request cycle (s) default %d", cycle);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -e, extended packet %d", PAYLOAD_SIZE_EXTEND);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -v, activate local log output");

    exit(EXIT_SUCCESS);
}

/*******************************************************************************
 *
 * main - main entry
 *
 * This is routine is entry point into the application.
 * It creates the client service application, issues a find and trigger a send
 * request when the found service is available.
 *
 * RETURNS: OK if application successfully terminate, ERROR parameter error
 * or can not create service application with success.
 */

int main(int argc, /* number of arguments */
    char* argv[] /* array of arguments */
)
{
    static const char opts[] = "s:S:i:I:m:M:p:z:c:ehf:v";
    int c;
    SERVICE_APPLICATION* pClApp = NULL;
    uint16_t i;
    uint16_t j;
    int val;
    STATUS rc;
    struct timespec delay;

    while ((c = getopt(argc, argv, opts)) != EOF) {
        switch (c) {
        case 's': /* service id */
        {
            if (sscanf(optarg, "0x%X", (unsigned int*)&val) != 1) {
                val = atoi(optarg);
            }
        }
            servId = (SD_SERVICE)val;
            break;
        case 'S': /* services range */
        {
            if (sscanf(optarg, "0x%X", (unsigned int*)&val) != 1) {
                val = atoi(optarg);
            }
        }
            servRange = (uint16_t)val;
            break;
        case 'i': /* instance id */
        {
            if (sscanf(optarg, "0x%X", (unsigned int*)&val) != 1) {
                val = atoi(optarg);
            }
            instId = (SD_INSTANCE)val;
        } break;
        case 'I': /* instances range */
        {
            if (sscanf(optarg, "0x%X", (unsigned int*)&val) != 1) {
                val = atoi(optarg);
            }
            instRange = (uint16_t)val;
        } break;
        case 'm': /* method id */
        {
            if (sscanf(optarg, "0x%X", (unsigned int*)&val) != 1) {
                val = atoi(optarg);
            }
            methId = (METHOD)val;
        } break;
        case 'M': /* instances range */
        {
            if (sscanf(optarg, "0x%X", (unsigned int*)&val) != 1) {
                val = atoi(optarg);
            }
            methRange = (uint16_t)val;
        } break;
        case 'p': /* protocol */
        {
            if (strncmp(proto[PROTO_UDP], optarg, strlen(proto[PROTO_UDP])) == 0) {
                reliable = FALSE;
            } else if (strncmp(proto[PROTO_TCP], optarg, strlen(proto[PROTO_TCP])) == 0) {
                reliable = TRUE;
            } else {
                usage(argv);
            }
            break;
        }
        case 'z': /* packet size */
        {
            val = atoi(optarg);
            if (val == 0) {
                usage(argv);
            }
            pktSz = val;
            break;
        }
        case 'c': /* cycle period */
        {
            val = atoi(optarg);
            if (val == 0) {
                usage(argv);
            }
            cycle = val;
            break;
        }
        case 'e': /* extended packet size for SOMEI/IP-TP */
        {
            pktSz = PAYLOAD_SIZE_EXTEND;
            break;
        }
        case 'f': /* log file */
        {
            const char* filename = optarg;
            SOMEIP_LOG(SOMEIP_LOG_DEBUG, "Logfile = %s", filename);
            logFD = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (logFD <= 0) {
                SOMEIP_LOG(SOMEIP_LOG_ERROR, "Fail to open log file %s", filename);
                exit(1);
            }
            break;
        }
        case 'v': /* activate local log output */
        {
            dlt_init();
            dlt_enable_local_print();
            dlt_set_resend_timeout_atexit(100);
            break;
        }
        default: {
            usage(argv);
            break;
        }
        }
    }

    /* prepare data to be sent */

    pldData = malloc(pktSz);
    if (pldData == NULL) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "Could not alloc payload memory");
        return ERROR;
    }
    for (i = 0; i < pktSz; i++) {
        pldData[i] = (uint8_t)(i % 256);
    }

    /* prepare expected response */

    for (i = 0; i < PAYLOAD_RESP_SIZE; ++i) {
        pldRespExp[i] = (uint8_t)(i % 256);
    }

    SOMEIP_LOG(SOMEIP_LOG_INFO, "Statistics alloc");
    /* alloc timestamps */

    pAvailTsArr = demoUtilTsAlloc((uint32_t)(servRange * instRange));
    pAvailArr = calloc((size_t)(servRange * instRange), sizeof(BOOL));
    SOMEIP_LOG(SOMEIP_LOG_INFO,
        "++ Availability [%d]*[%d] bytes [%d]*[%d]*[%lu]",
        servRange,
        instRange,
        servRange,
        instRange,
        sizeof(BOOL));
    pRespRecv = calloc((size_t)(servRange * instRange * methRange), sizeof(BOOL));
    pRespTsArr = demoUtilTsAlloc((uint32_t)(servRange * instRange * methRange));
    if ((NULL == pAvailTsArr) || (NULL == pAvailArr) || (NULL == pRespRecv) || (NULL == pRespTsArr)) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "Could not alloc statistics memory");
        doCleanup(pClApp);
        return ERROR;
    }

    SOMEIP_LOG(SOMEIP_LOG_INFO, "servAppNew()");
    pClApp = servAppNew("client");
    if (NULL == pClApp) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "servAppNew error");
        doCleanup(pClApp);
        return ERROR;
    }
    SOMEIP_LOG(SOMEIP_LOG_INFO, "servAppCreate()");
    rc = servAppCreate(pClApp);
    if (OK != rc) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "servAppCreate error [%d]", rc);
        doCleanup(pClApp);
        return ERROR;
    }

    /* handle all required service-ids and instance-ids */
    SOMEIP_LOG(SOMEIP_LOG_INFO, "handle all required service-ids and instance-ids");
    for (i = 0; i < servRange; i++) {
        for (j = 0; j < instRange; j++) {
            SOMEIP_LOG(SOMEIP_LOG_INFO, "Start handling Service [0x%04X] Instance [%#04X]", servId + i, instId + j);
            servAppRegAvailHnd(pClApp, servId + i, instId + j, &onAvailability);
            SOMEIP_LOG(SOMEIP_LOG_INFO, "servAppRegAvailHnd() done");
            servAppRegMsgHnd(pClApp, servId + i, instId + j, DEMO_METH, &onMessage);
            SOMEIP_LOG(SOMEIP_LOG_INFO, "servAppRegMsgHnd() done");
            demoUtilTsSet(&pAvailTsArr[i * instRange + j]);
            SOMEIP_LOG(SOMEIP_LOG_INFO, "demoUtilTsSet() done");
            servAppRequestServ(pClApp, servId + i, instId + j, DEMO_VER_MAJOR, DEMO_VER_MINOR);
            SOMEIP_LOG(SOMEIP_LOG_INFO, "Done handling Service [0x%04X] Instance [%#04X]", servId + i, instId + j);
        }
    }

    rc = servAppStart(pClApp);
    if (OK != rc) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "servAppStart error [%d]", rc);
        doCleanup(pClApp);
        return ERROR;
    }

    delay.tv_sec = MSEC_TO_SEC(1000);
    delay.tv_nsec = 0;
    /* wait for service to be available or for cycle seconds */
    while (!available && cycle > 0) {
        (void)nanosleep(&delay, NULL);
        cycle -= 1;
    }

    while (requests > 0) {
        for (i = 0; i < servRange; i++) {
            for (j = 0; j < instRange; j++) {
                if (pAvailArr[i * instRange + j]) {
                    sendRequest(pClApp, servId + i, instId + j);
                }
            }
        }
        (void)nanosleep(&delay, NULL);
        requests -= 1;
    }

    SOMEIP_LOG(SOMEIP_LOG_INFO, "Exit application");

    doCleanup(pClApp);

    return OK;
}

/*******************************************************************************
 *
 * doCleanup - application cleanup
 *
 * Clean all allocated memory during the application initialization and run.
 *
 * RETURNS: N/A
 */

LOCAL void doCleanup(SERVICE_APPLICATION* pServApp)
{
    SOMEIP_LOG(SOMEIP_LOG_DEBUG, "Do cleanup");
    if (NULL != pServApp) {
        servAppDelete(pServApp);
    }
    if (NULL != pldData) {
        free(pldData);
    }
    if (NULL != pAvailTsArr) {
        demoUtilTsFree(pAvailTsArr);
    }
    if (NULL != pAvailArr) {
        free(pAvailArr);
    }
    if (NULL != pRespRecv) {
        free(pRespRecv);
    }
    if (logFD)
        close(logFD);
}
