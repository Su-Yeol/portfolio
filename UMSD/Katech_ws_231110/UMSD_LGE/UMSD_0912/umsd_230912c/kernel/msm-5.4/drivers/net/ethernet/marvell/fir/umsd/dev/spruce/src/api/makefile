# Source files in this directory
TARGET = 
CSOURCES = Spruce_msdTCAM.c Spruce_msdTCAMRC.c Spruce_msdTCAMRCComp.c Spruce_msdBrgFdb.c \
	   Spruce_msdPolicy.c Spruce_msdBrgVtu.c Spruce_msdArp.c Spruce_msdBrgStu.c Spruce_msdPIRL.c Spruce_msdPortRmon.c \
	   Spruce_msdPortCtrl.c Spruce_msdEcid.c Spruce_msdPortPrioMap.c Spruce_msdQueueCtrl.c Spruce_msdSysCtrl.c \
	   Spruce_msdPhyCtrl.c Spruce_msdRMU.c Spruce_msdTCAMIntf.c \
	   Spruce_msdBrgFdbIntf.c Spruce_msdBrgVtuIntf.c Spruce_msdBrgStuIntf.c Spruce_msdPIRLIntf.c Spruce_msdQav.c \
	   Spruce_msdArpIntf.c Spruce_msdPortRmonIntf.c Spruce_msdPortCtrlIntf.c Spruce_msdPortPrioMapIntf.c \
	   Spruce_msdEcidIntf.c Spruce_msdQueueCtrlIntf.c Spruce_msdSysCtrlIntf.c Spruce_msdPhyCtrlIntf.c Spruce_msdPolicyIntf.c \
	   Spruce_msdTCAMRCIntf.c Spruce_msdRMUIntf.c Spruce_msdPTP.c Spruce_msdPTPIntf.c \
	   Spruce_msdLed.c Spruce_msdLedIntf.c Spruce_msdTCAMRCCompIntf.c Spruce_msdEEPROM.c Spruce_msdEEPROMIntf.c

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
OUT_DIR = $(TEMP_DIR)/dev/spruce/src/api

# Include common build rules
ifeq ($(OS_RUN),LINUX)
include $(MSD_TOOL_DIR)/makelnx.rules
endif

# end of file
