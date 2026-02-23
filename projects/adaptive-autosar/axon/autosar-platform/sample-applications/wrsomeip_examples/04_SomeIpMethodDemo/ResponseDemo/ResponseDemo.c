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
#include <time.h>

#include <demoUtil.h>
#include <wrsomeip/someipLogger.h>
#include <wrsomeip/wrsomeip.h>

/* defines */

#define SEND_RESPONSE 0
#define PROTO_UDP 0
#define PROTO_TCP 1
#define DEAFULT_REL PROTO_UDP
#define PAYLOAD_SIZE 120

/* locals */

LOCAL BOOL appRunning = TRUE;
LOCAL BOOL reliable = FALSE;
LOCAL char* proto[2] = {"UDP", "TCP"};
LOCAL int stopOfferDelay = DEFAULT_STOP_DELAY;
LOCAL uint8_t pldData[PAYLOAD_SIZE];
LOCAL SD_SERVICE servId = DEMO_SERV;
LOCAL uint16_t servRange = DEMO_SERV_RANGE;
LOCAL SD_INSTANCE instId = DEMO_INST;
LOCAL uint16_t instRange = DEMO_INST_RANGE;

/* forward declarations */

LOCAL void onMessage(SERVICE_APPLICATION* pSa, SOMEIP_MESSAGE* pMsgReq);
LOCAL void usage(char* argv[]);
LOCAL void doCleanup(SERVICE_APPLICATION* pSrvApp);

/****************************************************************************
 *
 * onMessage - SOME/IP messages handler
 *
 * This is routine is used to handle the received SOMEIP messages in the Server
 * side and responds back with a specific response pattern.
 *
 * RETURNS: N/A
 */

LOCAL void onMessage(SERVICE_APPLICATION* pSa, SOMEIP_MESSAGE* pMsgReq)
{
    SOMEIP_HEADER* pHdr;
    SOMEIP_MESSAGE* pMsgResp;
    SOMEIP_PAYLOAD* pPldResp;
    SOMEIP_PAYLOAD* pPld;
    STATUS rc;

    if ((pMsgReq == NULL) || (pMsgReq->pHdr == NULL) || (pMsgReq->pPld == NULL)) {
        return;
    }

    pHdr = pMsgReq->pHdr;
    pPld = pMsgReq->pPld;

    SOMEIP_LOG(SOMEIP_LOG_INFO,
        "MessageHandler for SOMEIP messages in the server "
        "side is triggered !");
    SOMEIP_LOG(SOMEIP_LOG_INFO,
        "Receive a request for service [0x%04X.0x%04X] instance "
        "[0x%04X] client/session [0x%04X.0x%04X] size [%d]",
        pHdr->service,
        pHdr->method,
        pHdr->instance,
        pHdr->client,
        pHdr->session,
        pPld->size);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "The server is sending now a response message!");

    pPldResp = someipPayloadNew(pldData, PAYLOAD_SIZE);
    if (pPldResp == NULL) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "Can not create response payload!");
        return;
    }

    pMsgResp = someipCreateResponse(pMsgReq);
    if (pMsgResp == NULL) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "Can not create response message!");
        someipPayloadDelete(pPldResp);
        return;
    }
    pMsgResp->pPld = pPldResp;

    rc = servAppSend(pSa, pMsgResp);
    if (rc != OK) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "Can not send response message!");
    }

    someipPayloadDelete(pPldResp);
    someipMessageDelete(pMsgResp, NOT_RCV_MSG);
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
        "usage: %s [-p <udp|tcp>] [-o <delay>] [-s <service-id>]"
        "[-S <service-id-range] [-i <instance-id>] "
        "[-I <instance-id-range>] [-v]",
        argv[0]);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "OPTIONS:");
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -p <UDP|TCP>, default %s", proto[DEAFULT_REL]);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -o <delay> - stop offer, default %d (ms)", stopOfferDelay);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -s <service-id> - service id, default 0x%04X", servId);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -S <service-range> - service id range, default %d", servRange);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -i <instance-id> - instance id, default 0x%04X", instId);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -I <instance-range> - instance id range, default %d", instRange);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -v, activate local log output");

    exit(EXIT_SUCCESS);
}

/*******************************************************************************
 *
 * main - main entry
 *
 * This is routine is entry point into the application.
 * It creates the server service application, register message handlers, offers
 * demo services and then wait for requests, responding back with a specific
 * pattern.
 *
 * RETURNS: OK if application successfully terminate, ERROR parameter error
 * or can not create service application with success.
 */

int main(int argc, /* number of arguments */
    char* argv[] /* array of arguments */
)
{
    static const char opts[] = "s:S:i:I:p:o:h:v";
    int c;
    SERVICE_APPLICATION* pSrvApp;
    uint16_t i;
    uint16_t j;
    int val;
    STATUS rc;
    struct timespec appLifeTime;

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
        case 'p': /* protocol */
        {
            if (strncmp(proto[PROTO_UDP], optarg, strlen(proto[PROTO_UDP])) == 0) {
                reliable = FALSE;
            } else if (strncmp(proto[PROTO_TCP], optarg, strlen(proto[PROTO_TCP])) == 0) {
                reliable = TRUE;
            } else {
                usage(argv);
                return ERROR;
            }
            break;
        }
        case 'o': /* stop offer delay */
        {
            stopOfferDelay = atoi(optarg);
        } break;
        case 'v': /* activate local log output */
        {
            dlt_init();
            dlt_enable_local_print();
            dlt_set_resend_timeout_atexit(100);
            break;
        }
        default: {
            usage(argv);
            return ERROR;
        }
        }
    }

    pSrvApp = servAppNew("server");
    if (NULL == pSrvApp) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "servAppNew error");
        doCleanup(pSrvApp);
        return ERROR;
    }

    rc = servAppCreate(pSrvApp);
    if (OK != rc) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "servAppCreate error [%d]", rc);
        doCleanup(pSrvApp);
        return ERROR;
    }

    /* prepare response payload */
    for (i = 0; i < PAYLOAD_SIZE; ++i) {
        pldData[i] = (uint8_t)(i % 256);
    }

    /* handle all required service-ids and instance-ids */

    for (i = 0; i < servRange; i++) {
        for (j = 0; j < instRange; j++) {
            servAppRegMsgHnd(pSrvApp, servId + i, instId + j, DEMO_METH, &onMessage);
            servAppOfferServ(pSrvApp, servId + i, instId + j, DEMO_VER_MAJOR, DEMO_VER_MINOR);
        }
    }

    rc = servAppStart(pSrvApp);
    if (OK != rc) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "servAppStart error [%d]", rc);
        doCleanup(pSrvApp);
        return ERROR;
    }

    /* wait a while and stop offering of the services */
    appLifeTime.tv_sec = MSEC_TO_SEC(stopOfferDelay);
    appLifeTime.tv_nsec = 0;
    (void)nanosleep(&appLifeTime, NULL);

    for (i = 0; i < servRange; i++) {
        for (j = 0; j < instRange; j++) {
            (void)servAppStopOfferServ(pSrvApp, servId + i, instId + j, DEMO_VER_MAJOR, DEMO_VER_MINOR);
        }
    }
    SOMEIP_LOG(SOMEIP_LOG_INFO, "Exit application");

    doCleanup(pSrvApp);

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
    SOMEIP_LOG(SOMEIP_LOG_INFO, "Do cleanup");
    if (NULL != pServApp) {
        servAppDelete(pServApp);
    }
}
