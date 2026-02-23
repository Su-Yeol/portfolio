#ifndef DCOMMUNICATOR_H
#define DCOMMUNICATOR_H

#include "DControlModule.h"
#include "DConfigParser.h"
#include "../shared/ModuleInterfaceCommon.h"
#include "../shared/communicator_cd.h"

MODULES_COMMON_COMM_DECLARE_BASE_HANDLERS
void GPSParser();
void PathReceiver();
void MobileyeReceiver();
void IbeoReceiver();
void RadarReceiver();

#endif
