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

#define PAYLOAD_SIZE 10
#define PAYLOAD_RESP_SIZE 120
#define PAYLOAD_NOTIF_SIZE 10

/* locals */

LOCAL uint32_t cycle = DEMO_CYCLE; /* cyclic check (s) */
LOCAL BOOL appRunning = TRUE;
LOCAL BOOL available = FALSE;
LOCAL BOOL testExtended = FALSE;
LOCAL BOOL reliable = FALSE;
LOCAL uint8_t* pldData = NULL;
LOCAL int pktSz = PAYLOAD_SIZE;
LOCAL BOOL respRecved = FALSE;
LOCAL uint8_t pldRespExp[PAYLOAD_RESP_SIZE];
LOCAL uint8_t pldNotifExp[PAYLOAD_NOTIF_SIZE];
LOCAL BOOL subAck = FALSE;
LOCAL SD_SERVICE servId = DEMO_SERV;
LOCAL uint16_t servRange = DEMO_SERV_RANGE;
LOCAL SD_INSTANCE instId = DEMO_INST;
LOCAL uint16_t instRange = DEMO_INST_RANGE;
LOCAL EVENT eventId = DEMO_EV;
LOCAL uint16_t eventRange = DEMO_EV_RANGE;
LOCAL EVGR evgrId = DEMO_EVGR;
LOCAL uint16_t evgrRange = DEMO_EVGR_RANGE;
LOCAL BOOL bTsPld = NOTIF_TS;

/* forward declarations */

LOCAL void onAvailability(SERVICE_APPLICATION* pSa, SERVICE service, INSTANCE instance, BOOL isAvail);
LOCAL void onMessage(SERVICE_APPLICATION* pSa, SOMEIP_MESSAGE* pMsg);
LOCAL void sendRequest(SERVICE_APPLICATION* pSrvApp);
LOCAL void doCleanup(SERVICE_APPLICATION* pSrvApp);
LOCAL void usage(char* argv[]);
LOCAL void onSubscribe(SERVICE_APPLICATION* pSa, SERVICE service, INSTANCE instance, EVGR evgr, SUBS_STATE subSt);

/*******************************************************************************
 *
 * subscribe - subscribe to event/event group
 *
 * This is routine is used to subscribe event/event group
 *
 * RETURNS: N/A
 */

LOCAL void subscribe(SERVICE_APPLICATION* pSrvApp, SERVICE serv, INSTANCE inst)
{
    STATUS rc;
    uint16_t i;

    SOMEIP_LOG(SOMEIP_LOG_INFO,
        "Client is Sending a SubscribeEventGroup message to the "
        "offering application!");

    /* handle all required service-ids and instance-ids */

    for (i = 0; i < servRange; i++) {
        uint16_t j;

        for (j = 0; j < instRange; j++) {
            uint16_t k;

            for (k = 0; k < evgrRange; k++) {
                uint16_t l;

                for (l = 0; l < eventRange; l++) {

                    rc = servAppRequestEvent(pSrvApp, serv + i, inst + j, eventId + l, evgrId + k, TRUE);
                    if (OK != rc) {
                        SOMEIP_LOG(SOMEIP_LOG_ERROR,
                            "Request event error "
                            "[0x%04X:0x%04X:0x%04X:0x%04X]",
                            eventId + l,
                            evgrId + k,
                            serv + i,
                            inst + j);
                        return;
                    }

                    rc = servAppSubscribe(pSrvApp, serv + i, inst + j, evgrId + k, DEMO_VER_MAJOR, eventId + l);
                    if (OK != rc) {
                        SOMEIP_LOG(SOMEIP_LOG_ERROR,
                            "Subscribe error "
                            "[0x%04X:0x%04X:0x%04X:0x%04X]",
                            eventId + l,
                            evgrId + k,
                            serv + i,
                            inst + j);
                        return;
                    }
                }
            }
        }
    }

    if (testExtended) {
        /*
         * Test sending subscribe to eventgroup with an unknown service ID,
         * it should fail
         */

        rc = servAppRequestEvent(pSrvApp, serv + 1, inst, eventId, evgrId, TRUE);
        if (OK != rc) {
            SOMEIP_LOG(
                SOMEIP_LOG_ERROR, "Request event error [0x%04X:0x%04X:0x%04X0x%04X", eventId, evgrId, serv + 1, inst);
        }

        rc = servAppSubscribe(pSrvApp, serv + 1, inst, evgrId, DEMO_VER_MAJOR, eventId);
        if (OK != rc) {
            SOMEIP_LOG(
                SOMEIP_LOG_ERROR, "Subscribe error [0x%04X:0x%04X:0x%04X0x%04X", eventId, evgrId, serv + 1, inst);
        }

        /*
         * Test sending subscribe to eventgroup with an unknown instance ID,
         * it should fail
         */

        rc = servAppRequestEvent(pSrvApp, serv, inst + 1, eventId, evgrId, TRUE);
        if (OK != rc) {
            SOMEIP_LOG(
                SOMEIP_LOG_ERROR, "Request event error [0x%04X:0x%04X:0x%04X0x%04X", eventId, evgrId, serv, inst + 1);
        }

        rc = servAppSubscribe(pSrvApp, serv, inst + 1, evgrId, DEMO_VER_MAJOR, eventId);
        if (OK != rc) {
            SOMEIP_LOG(
                SOMEIP_LOG_ERROR, "Subscribe error [0x%04X:0x%04X:0x%04X0x%04X", eventId, evgrId, serv, inst + 1);
        }

        /*
         * Test sending subscribe to eventgroup with a wrong EventGroup,
         * it should fail on server side, it should not receive an aknowledge
         */

        rc = servAppRequestEvent(pSrvApp, serv, inst, eventId, evgrId + 1, TRUE);
        if (OK != rc) {
            SOMEIP_LOG(
                SOMEIP_LOG_ERROR, "Request event error [0x%04X:0x%04X:0x%04X0x%04X", eventId, evgrId + 1, serv, inst);
        }

        rc = servAppSubscribe(pSrvApp, serv, inst, evgrId, DEMO_VER_MAJOR, eventId);
        if (OK != rc) {
            SOMEIP_LOG(
                SOMEIP_LOG_ERROR, "Subscribe error [0x%04X:0x%04X:0x%04X0x%04X", eventId, evgrId + 1, serv, inst);
        }
    }
    SOMEIP_LOG(SOMEIP_LOG_INFO, "Subscribe to Events finished, waiting for notifications.");
}

/*******************************************************************************
 *
 * sendRequest - send request message
 *
 * This is routine is used to send request message.
 *
 * RETURNS: N/A
 */

LOCAL void sendRequest(SERVICE_APPLICATION* pSrvApp)
{
    SOMEIP_MESSAGE* pMsgReq;
    SOMEIP_PAYLOAD* pPld;
    STATUS rc;

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

    pMsgReq->pHdr->service = servId;
    pMsgReq->pHdr->instance = instId;
    pMsgReq->pHdr->method = DEMO_METH;
    pMsgReq->pPld = pPld;

    rc = servAppSend(pSrvApp, pMsgReq);
    if (rc != OK) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "Can not send request message!");
    }

    someipPayloadDelete(pPld);
    someipMessageDelete(pMsgReq, NOT_RCV_MSG);
    respRecved = FALSE;
}

/*******************************************************************************
 *
 * onAvailability - Service Discovery messages handler
 *
 * This is routine is used to handle the received SD messages.
 *
 * RETURNS: N/A
 */

LOCAL void onAvailability(SERVICE_APPLICATION* pSrvApp, SERVICE serv, INSTANCE inst, BOOL isAvail)
{
    SOMEIP_LOG(SOMEIP_LOG_INFO,
        "Client Service [0x%04X:0x%04X] is %s!",
        serv,
        inst,
        isAvail ? "Available" : "Not Available anymore");

    if (isAvail) {
        available = TRUE;

        subscribe(pSrvApp, serv, inst);
    }
}

/*******************************************************************************
 *
 * logPayload - print message payload
 *
 * This is routine is used to log the specified payload.
 *
 * RETURNS: N/A
 */

LOCAL void logPayload(SOMEIP_PAYLOAD* pPld)
{
    SOMEIP_LOG(SOMEIP_LOG_INFO, "Print payload size [%d]", pPld->size);

    someipBufferPrint("Payload", pPld->pData, pPld->size, PRINT_MODE_HEX);
}

/*******************************************************************************
 *
 * logTimestamp - log timestamp
 *
 * This is routine is used to log notification timestamp.
 *
 * RETURNS: N/A
 */

LOCAL void logTimestamp(SOMEIP_MESSAGE* pMsg)
{
    DEMO_TS ts;
    int us;

    if (pMsg->pPld->size != NOTIF_TS_SZ) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "Notification timestamp payload wrong size [%d]", pMsg->pPld->size);
        return;
    }

    if (EOF != sscanf((char*)pMsg->pPld->pData, NOTIF_TS_FMT, &ts.sec, &ts.usec)) {
        us = demoUtilTsDif(&ts, NULL);

        SOMEIP_LOG(SOMEIP_LOG_INFO,
            "Receive a notification for event [0x%04X.0x%04X.0x%04X] in "
            "[%dsec %dms %dus]",
            pMsg->pHdr->service,
            pMsg->pHdr->instance,
            pMsg->pHdr->method,
            us / USEC_PER_SEC,
            (us / USEC_PER_MSEC) % USEC_PER_MSEC,
            us % USEC_PER_MSEC);
    } else {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "Notification timestamp can't be extracted from payload");
    }
}

/*******************************************************************************
 *
 * onMessage - SOME/IP messages handler
 *
 * This is routine is used to handle the received SOMEIP Event messages in the
 * Client side.
 *
 * RETURNS: N/A
 */

void onMessage(SERVICE_APPLICATION* pSa, SOMEIP_MESSAGE* pMsg)
{
    SOMEIP_HEADER* pHdr;
    SOMEIP_PAYLOAD* pPld;

    (void)pSa;

    pHdr = pMsg->pHdr;
    pPld = pMsg->pPld;

    SOMEIP_LOG(SOMEIP_LOG_INFO,
        "Received a registered message service [0x%04X:0x%04X] "
        "client/session [0x%04X:0x%04X] size [%d]",
        pHdr->service,
        pHdr->instance,
        pHdr->client,
        pHdr->session,
        pPld->size);

    if (pMsg->pHdr->msgType == E_RESPONSE) {
        SOMEIP_LOG(SOMEIP_LOG_INFO, "Response");
        if (memcmp(pPld->pData, pldRespExp, PAYLOAD_RESP_SIZE) == 0) {
            SOMEIP_LOG(SOMEIP_LOG_INFO, "PASS client side response");
        } else {
            SOMEIP_LOG(SOMEIP_LOG_ERROR, "FAIL client side response");
        }
        respRecved = TRUE;
    } else if (pMsg->pHdr->msgType == E_NOTIFICATION) {
        SOMEIP_LOG(SOMEIP_LOG_INFO,
            "Received a notification for Event [0x%04X:0x%04X:0x%04X]",
            pMsg->pHdr->service,
            pMsg->pHdr->instance,
            pMsg->pHdr->method);

        if (bTsPld) /* payload contains notification timestamp */
        {
            logTimestamp(pMsg);
            return;
        }

        logPayload(pMsg->pPld);
        if (memcmp((const char*)pPld->pData,
                pldNotifExp,
                strlen((char*)pPld->pData) > PAYLOAD_NOTIF_SIZE ? PAYLOAD_NOTIF_SIZE : strlen((char*)pPld->pData))
            == 0) {
            SOMEIP_LOG(SOMEIP_LOG_INFO, "PASS client side notification");
        } else {
            SOMEIP_LOG(SOMEIP_LOG_ERROR, "FAIL client side notification");
        }
    }
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
        "usage: %s [-s <service>] [-S <service-range] [-i <instance>] "
        "[-I <instance-range>] [-e <event] [-E <event-range>]"
        "[-g <event-group>] [-G <event-group-range>] [-c <cycle>] [-x] "
        "[-t] [-v]",
        argv[0]);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "OPTIONS:");
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -s <service-id> - service-id, default 0x%04X", servId);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -S <service-range> - service-id range, default %d", servRange);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -i <instance-id> - instance-id, default 0x%04X", instId);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -I <instance-range> - instance-id range, default %d", instRange);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -e <event> - event id, default 0x%04X", eventId);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -E <event-range> - event id range, default %d", eventRange);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -g <event-group> - event group id, default 0x%04X", evgrId);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -G <event-group-range> - event group id range, default %d", evgrRange);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -c <cycle> - notify cycle time, default %d (ms)", cycle);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -t - timestamp payload");
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -x - extended Test mode");
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -v, activate local log output");

    exit(EXIT_SUCCESS);
}

/*******************************************************************************
 *
 * main - entry point for client event demo application
 *
 * This is the entry point into the client application example.
 * It creates the service application and tries to find a service.
 * If the service is found, it will subscribe to an eventgroup and wait for
 * event notifications.
 *
 * RETURNS:
 * - OK - application finished successfully
 * - ERROR - service application can not be started
 */

int main(int argc, /* number of arguments */
    char* argv[] /* array of arguments */
)
{
    int c;
    static const char opts[] = "s:S:i:I:e:E:g:G:c:txh:v";
    SERVICE_APPLICATION* pClientApp;
    STATUS rc;
    struct timespec appLifeTime;
    uint16_t i, j;
    int val;

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
            servRange = (SD_SERVICE)val;
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
            instRange = (SD_INSTANCE)val;
        } break;
        case 'e': /* event id */
        {
            if (sscanf(optarg, "0x%X", (unsigned int*)&val) != 1) {
                val = atoi(optarg);
            }
        }
            eventId = (EVENT)val;
            break;
        case 'E': /* event id range */
        {
            if (sscanf(optarg, "0x%X", (unsigned int*)&val) != 1) {
                val = atoi(optarg);
            }
            eventRange = (EVENT)val;
        } break;
        case 'g': /* event group id */
        {
            if (sscanf(optarg, "0x%X", (unsigned int*)&val) != 1) {
                val = atoi(optarg);
            }
        }
            evgrId = (EVGR)val;
            break;
        case 'G': /* event group id range */
        {
            if (sscanf(optarg, "0x%X", (unsigned int*)&val) != 1) {
                val = atoi(optarg);
            }
            evgrRange = (EVGR)val;
        } break;
        case 'c': {
            val = atoi(optarg);
            if (val == 0) {
                usage(argv);
            }
            cycle = val;
            break;
        }
        case 't': /* notification timestamp */
        {
            bTsPld = TRUE;
            break;
        }
        case 'x': {
            SOMEIP_LOG(SOMEIP_LOG_INFO, "Testing in extended mode");
            testExtended = TRUE;
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
            return ERROR;
        }
        }
    }

    /* prepare data to be sent */

    pldData = malloc(pktSz);
    if (pldData == NULL) {
        return ERROR;
    }
    for (i = 0; i < pktSz; i++) {
        pldData[i] = (uint8_t)(i % 256);
    }

    /* prepare expected response & notification */

    for (i = 0; i < PAYLOAD_RESP_SIZE; ++i) {
        pldRespExp[i] = (uint8_t)(i % 256);
    }

    for (i = 0; i < PAYLOAD_NOTIF_SIZE; ++i) {
        pldNotifExp[i] = (uint8_t)(i % 256);
    }

    pClientApp = servAppNew("SubscribeDemo");
    if (pClientApp == NULL) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "servAppNew error");
        doCleanup(pClientApp);
        return ERROR;
    }

    rc = servAppCreate(pClientApp);
    if (rc == ERROR) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "servAppCreate error");
        doCleanup(pClientApp);
        return rc;
    }

    rc = servAppStart(pClientApp);
    if (OK != rc) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "servAppStart error [%d]", rc);
        doCleanup(pClientApp);
        return ERROR;
    }

    /* handle all required service-ids and instance-ids */

    for (i = 0; i < servRange; i++) {
        for (j = 0; j < instRange; j++) {
            servAppRegAvailHnd(pClientApp, servId + i, instId + j, &onAvailability);
            servAppRegMsgHnd(pClientApp, servId + i, instId + j, SOMEIP_ANY_METHOD, &onMessage);
            servAppRegSubHnd(pClientApp, servId, instId, SOMEIP_ANY_METHOD, &onSubscribe);
            servAppRequestServ(pClientApp, servId + i, instId + j, DEMO_VER_MAJOR, DEMO_VER_MINOR);
        }
    }

    appLifeTime.tv_sec = MSEC_TO_SEC(cycle);
    appLifeTime.tv_nsec = 0;

    (void)nanosleep(&appLifeTime, NULL);

    /* unsubscribe from event groups if the service became available and thus we subscribed to it. */
    if (available) {
        for (i = 0; i < servRange; i++) {
            for (j = 0; j < instRange; j++) {
                uint16_t k;
                for (k = 0; k < evgrRange; k++) {
                    /* at the state of writing this call is not implemented, but this is how it could look like in the
                     * application side. */
                    rc = servAppUnsubscribe(pClientApp, servId + i, instId + j, evgrId + k);
                    if (OK != rc) {
                        SOMEIP_LOG(SOMEIP_LOG_ERROR,
                            "Unsubscribe error [0x%04X:0x%04X:0x%04X]",
                            evgrId + k,
                            servId + i,
                            instId + j);
                    }
                }
            }
        }
    }

    doCleanup(pClientApp);

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
    free(pldData);
}

/*******************************************************************************
 *
 * onSubscribe - on subscribe status
 *
 * This function is called by the service application when subscription state
 * is available acknowledged or not acknowledged for the specified event group.
 *
 * RETURNS: N/A
 */

LOCAL void onSubscribe(SERVICE_APPLICATION* pSa, SERVICE service, INSTANCE instance, EVGR evgr, SUBS_STATE subSt)
{
    char* ackStr;

    (void)pSa;
    if (SUBS_STATE_ACK == subSt) {
        ackStr = "accepted";
        subAck = TRUE;
    } else if (SUBS_STATE_NACK == subSt) {
        ackStr = "not accepted";
    } else {
        ackStr = "N/A";
    }

    SOMEIP_LOG(
        SOMEIP_LOG_INFO, "Event group [0x%04X:0x%04X:0x%04X] subscription [%s]", evgr, service, instance, ackStr);
}
