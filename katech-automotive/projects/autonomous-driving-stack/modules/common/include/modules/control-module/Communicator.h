#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include "ControlModule.h"
#include "ConfigParser.h"
#include "../shared/ModuleInterfaceCommon.h"
#include "../shared/communicator_cd.h"

MODULES_COMMON_COMM_EXTERN_FLAGS
MODULES_COMMON_COMM_EXTERN_STATE(GPSVariable, VehicleVariable, GlobalPathVariable, LocalPathVariable, ControlVariable)

MODULES_COMMON_COMM_DECLARE_BASE_HANDLERS
void IbeoReceiver();
void GPSParser();
void PathReceiver();

#endif
