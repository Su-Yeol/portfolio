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
#include <string.h>
#include <time.h>

#include <demoUtil.h>
#include <wrsomeip/someipLogger.h>
#include <wrsomeip/wrsomeip.h>

/* defines */

#define PAYLOAD_SIZE 120

/* locals */

LOCAL BOOL testExtended = FALSE;
LOCAL BOOL subscribed = FALSE;
LOCAL uint8_t pldData[PAYLOAD_SIZE];
LOCAL uint32_t evSize = NOTIF_PLD_SIZE_INIT;
LOCAL uint32_t cycle = DEMO_CYCLE / 1000;
LOCAL SD_SERVICE servId = DEMO_SERV;
LOCAL SD_SERVICE servRange = DEMO_SERV_RANGE;
LOCAL SD_INSTANCE instId = DEMO_INST;
LOCAL SD_INSTANCE instRange = DEMO_INST_RANGE;
LOCAL EVENT eventId = DEMO_EV;
LOCAL uint16_t eventRange = DEMO_EV_RANGE;
LOCAL EVGR evgrId = DEMO_EVGR;
LOCAL uint16_t evgrRange = DEMO_EVGR_RANGE;
LOCAL BOOL bTsPld = NOTIF_TS;

/* forward declarations */

LOCAL STATUS notify(SERVICE_APPLICATION* pServApp);
LOCAL void doCleanup(SERVICE_APPLICATION* pSrvApp);
LOCAL void sendResponse(SERVICE_APPLICATION* pSa, SOMEIP_MESSAGE* pMsg);
LOCAL void usage(char* argv[]);
LOCAL void onSubscribe(SERVICE_APPLICATION* pSa, SERVICE service, INSTANCE instance, EVGR evgr, SUBS_STATE subSt);

/*******************************************************************************
 *
 * onAvailability - Service Discovery messages handler
 *
 * This is routine is used to handle the received SD messages.
 *
 * RETURNS: N/A
 */

void onAvailability(SERVICE_APPLICATION* pServApp, SERVICE service, INSTANCE instance, BOOL isAvailable)
{
    (void)pServApp;

    SOMEIP_LOG(SOMEIP_LOG_INFO,
        "Server Service [%#04x] Instance [%#04x] is %s!",
        service,
        instance,
        isAvailable ? "Available" : "Not Available anymore");
}

/*******************************************************************************
 *
 * onMessage - SOME/IP messages handler
 *
 * This is routine is used to handle the received SOMEIP messages in the Server
 * side.
 *
 * RETURNS: N/A
 */

void onMessage(SERVICE_APPLICATION* pServApp, SOMEIP_MESSAGE* pMsg)
{
    SOMEIP_LOG(SOMEIP_LOG_INFO,
        "Server received a message type [%04X] from client [%04x]",
        pMsg->pHdr->msgType,
        pMsg->pHdr->client);

    if (SOMEIP_MSG_IS_REQ_ANY(pMsg)) {
        SOMEIP_LOG(SOMEIP_LOG_INFO,
            "Server received a request service [%04X:%04X:%04X] size"
            "[%d]",
            pMsg->pHdr->service,
            pMsg->pHdr->instance,
            pMsg->pHdr->method,
            pMsg->pPld->size);

        if (SOMEIP_MSG_IS_REQ(pMsg)) {
            SOMEIP_LOG(SOMEIP_LOG_INFO, "Server sending response");
            sendResponse(pServApp, pMsg);
        }
    }
}

/*******************************************************************************
 *
 * notify - notify events
 *
 * This is routine is send demo notifications on a specific event group.
 *
 * RETURNS: N/A
 */

LOCAL STATUS notify(SERVICE_APPLICATION* pServApp)
{
    STATUS retVal = OK;
    SOMEIP_PAYLOAD* pPld1;
    SOMEIP_PAYLOAD* pPld2;
    uint16_t i;
    uint16_t j;
    uint16_t k;

    pPld2 = someipPayloadNew(pldData, evSize);

    if (bTsPld) {
        char tsStr[NOTIF_TS_SZ];
        DEMO_TS ts;

        demoUtilTsSet(&ts);
        (void)snprintf(tsStr, NOTIF_TS_SZ, NOTIF_TS_FMT, ts.sec, ts.usec);
        pPld1 = someipPayloadNew((uint8_t*)tsStr, NOTIF_TS_SZ);
    } else {
        pPld1 = someipPayloadNew(pldData, evSize);
    }

    if ((NULL == pPld1) || (NULL == pPld2)) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "Can't allocate test someip payloads");
        someipPayloadDelete(pPld1);
        someipPayloadDelete(pPld2);
        return ERROR;
    }

    SOMEIP_LOG(SOMEIP_LOG_INFO, "Setting event, length [%d]", evSize);

    for (i = 0; i < servRange; i++) {
        for (j = 0; j < instRange; j++) {
            for (k = 0; k < eventRange; k++) {
                STATUS rc = servAppNotify(pServApp, servId + i, instId + j, eventId + k, pPld1);
                if (OK != rc) {
                    SOMEIP_LOG(SOMEIP_LOG_ERROR,
                        "Test notify event [0x%04X:0x%04X:0x%04X]: "
                        "FAILED",
                        eventId,
                        servId,
                        instId);
                    retVal = rc;
                } else {
                    SOMEIP_LOG(SOMEIP_LOG_INFO,
                        "Test notify event [0x%04X:0x%04X:0x%04X]: "
                        "PASSED",
                        eventId,
                        servId,
                        instId);
                }
            }
        }
    }

    someipPayloadDelete(pPld1);

    if (testExtended) {

        /*
         * Test sending notification for a wrong service (not offered),
         * it should fail
         */

        STATUS rc = servAppNotify(pServApp, servId + 1, instId, eventId, pPld2);
        if (rc == OK) {
            SOMEIP_LOG(SOMEIP_LOG_ERROR,
                "Test notify event on not offered service "
                "[0x%04X:0x%04X:0x%04X]: FAILED",
                eventId,
                servId + 1,
                instId);
            retVal = ERROR;
        } else {
            SOMEIP_LOG(SOMEIP_LOG_INFO,
                "Test notify event on not offered service "
                "[0x%04X:0x%04X:0x%04X]: PASSED",
                eventId,
                servId + 1,
                instId);
        }

        /*
         * Test sending notification for a wrong instance (not offered),
         * it should fail
         */

        rc = servAppNotify(pServApp, servId, instId + 1, eventId, pPld2);
        if (rc == OK) {
            SOMEIP_LOG(SOMEIP_LOG_ERROR,
                "Test notify event on not offered instance "
                "[0x%04X:0x%04X:0x%04X]: FAILED",
                eventId,
                servId,
                instId + 1);
            retVal = ERROR;
        } else {
            SOMEIP_LOG(SOMEIP_LOG_INFO,
                "Test notify event on not offered instance "
                "[0x%04X:0x%04X:0x%04X]: PASSED",
                eventId,
                servId,
                instId + 1);
        }

        /*
         * Test sending notification with a wrong event id (not offered),
         * it should fail
         */

        rc = servAppNotify(pServApp, servId, instId, eventId + 1, pPld2);
        if (rc == OK) {
            SOMEIP_LOG(SOMEIP_LOG_ERROR,
                "Test notify on not offered event "
                "[0x%04X:0x%04X:0x%04X]: FAILED",
                eventId + 1,
                servId,
                instId);
            retVal = ERROR;
        } else {
            SOMEIP_LOG(SOMEIP_LOG_INFO,
                "Test notify on not offered event "
                "[0x%04X:0x%04X:0x%04X]: PASSED",
                eventId + 1,
                servId,
                instId);
        }
    }

    someipPayloadDelete(pPld2);

    evSize++;
    if (evSize > NOTIF_PLD_SIZE_MAX) {
        evSize = NOTIF_PLD_SIZE_INIT;
    }
    return retVal;
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
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -c <cycle> - notify cycle time, default %d (s)", cycle);
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -t - timestamp payload");
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -x - extended Test mode");
    SOMEIP_LOG(SOMEIP_LOG_INFO, "  -v, activate local log output");

    exit(EXIT_SUCCESS);
}

/*******************************************************************************
 *
 * main - entry point for server event demo application
 *
 * This is the entry point into the server application example.
 * It creates the service application and offers a service.
 * It will wait for a number of event subscriptions to which it responds with
 * event subscribe acknowledge. After all the clients subscribed it will send
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
    SERVICE_APPLICATION* pSrvApp;
    STATUS rc;
    STATUS retValNot = OK;
    struct timespec cycleDelay;
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
        case 'c': /* run cycle */
        {
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

    pSrvApp = servAppNew("NotifyDemo");
    if (NULL == pSrvApp) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "servAppNew error");
        doCleanup(pSrvApp);
        return ERROR;
    }

    rc = servAppCreate(pSrvApp);
    if (rc != OK) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "servAppCreate error");
        doCleanup(pSrvApp);
        return rc;
    }

    rc = servAppStart(pSrvApp);
    if (rc != OK) {
        SOMEIP_LOG(SOMEIP_LOG_ERROR, "servAppStart error [%d]", rc);
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
            uint16_t k;

            servAppRegMsgHnd(pSrvApp, servId + i, instId + j, DEMO_METH, &onMessage);
            servAppRegSubHnd(pSrvApp, servId, instId, SOMEIP_ANY_METHOD, &onSubscribe);
            rc = servAppOfferServ(pSrvApp, servId + i, instId + j, DEMO_VER_MAJOR, DEMO_VER_MINOR);
            if (rc != OK) {
                SOMEIP_LOG(SOMEIP_LOG_ERROR, "servAppOfferServ [0x%04X|0x%04X] error [%d]", servId + i, instId + j, rc);
                doCleanup(pSrvApp);
                return ERROR;
            }

            for (k = 0; k < evgrRange; k++) {
                uint16_t l;

                for (l = 0; l < eventRange; l++) {
                    rc = servAppOfferEvent(pSrvApp, servId + i, instId + j, eventId + l, evgrId + k, TRUE);
                    if (rc != OK) {
                        SOMEIP_LOG(SOMEIP_LOG_ERROR,
                            "servAppStopOfferEvent [0x%04X|0x%04X]:[0x%04X|0x%04X] error [%d]",
                            servId + i,
                            instId + j,
                            eventId + l,
                            evgrId + k,
                            rc);
                        doCleanup(pSrvApp);
                        return ERROR;
                    }
                }
            }
        }
    }

    /* wait for SubscribeDemo to be started, then start notifing */
    cycleDelay.tv_sec = MSEC_TO_SEC(1000);
    cycleDelay.tv_nsec = 0;
    while (!subscribed && cycle > 0) {
        (void)nanosleep(&cycleDelay, NULL);
        cycle -= 1;
    }
    while (OK == retValNot && cycle > 0 && subscribed) {
        retValNot = notify(pSrvApp);
        cycle -= 1;
        (void)nanosleep(&cycleDelay, NULL);
    }

    for (i = 0; i < servRange; i++) {
        for (j = 0; j < instRange; j++) {
            uint16_t k;
            for (k = 0; k < evgrRange; k++) {
                uint16_t l;
                for (l = 0; l < eventRange; l++) {
                    rc = servAppStopOfferEvent(pSrvApp, servId + i, instId + j, eventId + l, evgrId + k);
                    if (rc != OK) {
                        SOMEIP_LOG(SOMEIP_LOG_ERROR,
                            "servAppStopOfferEvent [0x%04X|0x%04X]:[0x%04X|0x%04X] error [%d]",
                            servId + i,
                            instId + j,
                            eventId + l,
                            evgrId + k,
                            rc);
                        doCleanup(pSrvApp);
                        return ERROR;
                    }
                }
            }
            rc = servAppStopOfferServ(pSrvApp, servId + i, instId + j, DEMO_VER_MAJOR, DEMO_VER_MINOR);
            if (rc != OK) {
                SOMEIP_LOG(
                    SOMEIP_LOG_ERROR, "servAppStopOfferServ [0x%04X|0x%04X] error [%d]", servId + i, instId + j, rc);
                doCleanup(pSrvApp);
                return ERROR;
            }
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

/*******************************************************************************
 *
 * sendResponse - send response to a request
 *
 * Create a response message and send it back to the client that sent the
 * request.
 *
 * RETURNS: N/A
 */

LOCAL void sendResponse(SERVICE_APPLICATION* pSa, SOMEIP_MESSAGE* pMsgReq)
{
    SOMEIP_MESSAGE* pMsgResp;
    SOMEIP_PAYLOAD* pPldResp;
    STATUS rc;

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
        subscribed = TRUE;
    } else if (SUBS_STATE_NACK == subSt) {
        ackStr = "not accepted";
        subscribed = FALSE;
    } else {
        ackStr = "N/A";
    }

    SOMEIP_LOG(
        SOMEIP_LOG_INFO, "Event group [0x%04X:0x%04X:0x%04X] subscription [%s]", service, instance, evgr, ackStr);
}
