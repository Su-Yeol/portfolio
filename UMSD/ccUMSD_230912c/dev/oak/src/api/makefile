# Source files in this directory
TARGET = 
CSOURCES = Oak_msdTCAM.c Oak_msdTCAMRC.c Oak_msdTCAMRCComp.c Oak_msdBrgFdb.c \
	   Oak_msdPolicy.c Oak_msdBrgVtu.c Oak_msdArp.c Oak_msdBrgStu.c Oak_msdPIRL.c Oak_msdPortRmon.c \
	   Oak_msdPortCtrl.c Oak_msdEcid.c Oak_msdPortPrioMap.c Oak_msdQueueCtrl.c Oak_msdSysCtrl.c \
	   Oak_msdPhyCtrl.c Oak_msdRMU.c Oak_msdTCAMIntf.c \
	   Oak_msdBrgFdbIntf.c Oak_msdBrgVtuIntf.c Oak_msdBrgStuIntf.c Oak_msdPIRLIntf.c Oak_msdQav.c \
	   Oak_msdArpIntf.c Oak_msdPortRmonIntf.c Oak_msdPortCtrlIntf.c Oak_msdPortPrioMapIntf.c \
	   Oak_msdEcidIntf.c Oak_msdQueueCtrlIntf.c Oak_msdSysCtrlIntf.c Oak_msdPhyCtrlIntf.c Oak_msdPolicyIntf.c \
	   Oak_msdTCAMRCIntf.c Oak_msdRMUIntf.c Oak_msdPTP.c Oak_msdPTPIntf.c \
	   Oak_msdLed.c Oak_msdLedIntf.c Oak_msdTCAMRCCompIntf.c Oak_msdEEPROM.c Oak_msdEEPROMIntf.c

ASOURCES	=

# Include common variable definitions
ifeq ($(OS_RUN),LINUX)
include $(MSD_TOOL_DIR)/makelnx.defs
endif

# Add in extra stuffs
EXTRA_INCLUDE	+=
EXTRA_DEFINE	+=
ADDED_CFLAGS	+=

#Output directory
OUT_DIR = $(TEMP_DIR)/dev/oak/src/api

# Include common build rules
ifeq ($(OS_RUN),LINUX)
include $(MSD_TOOL_DIR)/makelnx.rules
endif

# end of file
