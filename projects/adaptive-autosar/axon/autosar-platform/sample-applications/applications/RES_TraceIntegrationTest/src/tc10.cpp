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

#include "tc10.h"
#include "apd/wgres/log/messages.h"
#include <ara/core/initialization.h>
#include <ara/log/logger.h>

// void ArtiTaskSwitch(CallingContext callingContext, uint32_t coreId, uint32_t nextId);
void emit_trace_message_ostaskschedule(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t nextid);
void emit_trace_message_ostaskschedule(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t nextid)
{
    logger.Log(apd::wgres::log::OsTaskSchedule, timestamp, coreid, nextid);
}

// void ArtiTaskWait(CallingContext callingContext, uint32_t coreId, uint32_t taskId);
void emit_trace_message_ostaskwait(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t taskid);
void emit_trace_message_ostaskwait(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t taskid)
{
    logger.Log(apd::wgres::log::OsTaskWait, timestamp, coreid, taskid);
}

// void ArtiTaskRelease(CallingContext callingContext, uint32_t coreId, uint32_t taskId);
void emit_trace_message_ostaskrelease(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t taskid);
void emit_trace_message_ostaskrelease(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t taskid)
{
    logger.Log(apd::wgres::log::OsTaskRelease, timestamp, coreid, taskid);
}

// void ArtiTaskPreempt(CallingContext callingContext, uint32_t coreId, uint32_t taskId);
void emit_trace_message_ostaskpreempt(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t taskid);
void emit_trace_message_ostaskpreempt(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t taskid)
{
    logger.Log(apd::wgres::log::OsTaskPreempt, timestamp, coreid, taskid);
}

// void ArtiTaskExit(CallingContext callingContext, uint32_t coreId, uint32_t taskId);
void emit_trace_message_ostaskterminate(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t taskid);
void emit_trace_message_ostaskterminate(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t taskid)
{
    logger.Log(apd::wgres::log::OsTaskTerminate, timestamp, coreid, taskid);
}

// void ArtiTaskCreate(CallingContext callingContext, uint32_t coreId, uint32_t processId, uint32_t taskId);
void emit_trace_message_ostaskcreate(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t processid,
    std::uint32_t taskid);
void emit_trace_message_ostaskcreate(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t processid,
    std::uint32_t taskid)
{
    logger.Log(apd::wgres::log::OsTaskCreate, timestamp, coreid, processid, taskid);
}

// void ArtiTaskRename(CallingContext callingContext, uint32_t taskId, const char* taskName);
void emit_trace_message_ostaskrename(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t taskid,
    const char* taskname);
void emit_trace_message_ostaskrename(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t taskid,
    const char* taskname)
{
    logger.Log(apd::wgres::log::OsTaskRename, timestamp, taskid, taskname);
}

// void ArtiTaskInfo(CallingContext callingContext, uint32_t taskId, uint32_t processId, const char* taskName);
void emit_trace_message_ostaskinfo(ara::log::Logger& logger,
    std::uint32_t taskid,
    std::uint32_t processid,
    const char* taskname);
void emit_trace_message_ostaskinfo(ara::log::Logger& logger,
    std::uint32_t taskid,
    std::uint32_t processid,
    const char* taskname)
{
    logger.Log(apd::wgres::log::OsTaskInfo, taskid, processid, taskname);
}

// void ArtiProcessSwitch(CallingContext callingContext, uint32_t coreId, uint32_t nextId);
void emit_trace_message_osprocessswitch(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t nextid);
void emit_trace_message_osprocessswitch(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t nextid)
{
    logger.Log(apd::wgres::log::OsProcessSwitch, timestamp, coreid, nextid);
}
// void ArtiProcessCreate(CallingContext callingContext, uint32_t coreId, uint32_t processId, uint32_t parentId);
void emit_trace_message_osprocesscreate(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t processid,
    std::uint32_t parentid);
void emit_trace_message_osprocesscreate(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t processid,
    std::uint32_t parentid)
{
    logger.Log(apd::wgres::log::OsProcessCreate, timestamp, coreid, processid, parentid);
}

// void ArtiProcessDestroy(CallingContext callingContext, uint32_t coreId, uint32_t processId);
void emit_trace_message_osprocessdestroy(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t processid);
void emit_trace_message_osprocessdestroy(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t coreid,
    std::uint32_t processid)
{
    logger.Log(apd::wgres::log::OsProcessDestroy, timestamp, coreid, processid);
}

// void ArtiProcessRename(CallingContext callingContext, uint32_t processId, const char* processName);
void emit_trace_message_osprocessrename(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t processid,
    const char* processname);
void emit_trace_message_osprocessrename(ara::log::Logger& logger,
    std::uint64_t timestamp,
    std::uint32_t processid,
    const char* processname)
{
    logger.Log(apd::wgres::log::OsProcessRename, timestamp, processid, processname);
}

// void ArtiProcessInfo(CallingContext callingContext, uint32_t processId, uint32_t parentId, const char* processName);
void emit_trace_message_osprocessinfo(ara::log::Logger& logger,
    std::uint32_t processid,
    std::uint32_t parentid,
    const char* processname);
void emit_trace_message_osprocessinfo(ara::log::Logger& logger,
    std::uint32_t processid,
    std::uint32_t parentid,
    const char* processname)
{
    logger.Log(apd::wgres::log::OsProcessInfo, processid, parentid, processname);
}

void testcase10()
{
    ara::log::Logger& logger = ara::log::CreateLogger("TC10", "WG-RES TC10", ara::log::LogLevel::kVerbose);
    logger.LogInfo() << "test case 10";

    emit_trace_message_ostaskschedule(logger, 1, 1, 2);
    emit_trace_message_ostaskwait(logger, 2, 0, 1);
    emit_trace_message_ostaskrelease(logger, 3, 0, 1);
    emit_trace_message_ostaskpreempt(logger, 4, 0, 1);
    emit_trace_message_ostaskterminate(logger, 5, 0, 1);
    emit_trace_message_ostaskcreate(logger, 6, 0, 1, 2);
    emit_trace_message_ostaskrename(logger, 6, 1, "taskname");
    emit_trace_message_ostaskinfo(logger, 1, 2, "taskname");
    emit_trace_message_osprocessswitch(logger, 8, 1, 2);
    emit_trace_message_osprocesscreate(logger, 9, 1, 2, 3);
    emit_trace_message_osprocessdestroy(logger, 10, 0, 1);
    emit_trace_message_osprocessrename(logger, 11, 1, "processname");
    emit_trace_message_osprocessinfo(logger, 1, 2, "processname");
}
