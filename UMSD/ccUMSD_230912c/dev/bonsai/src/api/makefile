# Source files in this directory
TARGET = 
CSOURCES = Bonsai_msdTCAM.c Bonsai_msdTCAMRC.c Bonsai_msdTCAMRCComp.c Bonsai_msdBrgFdb.c \
	   Bonsai_msdPolicy.c Bonsai_msdBrgVtu.c Bonsai_msdArp.c Bonsai_msdBrgStu.c Bonsai_msdPIRL.c Bonsai_msdPortRmon.c \
	   Bonsai_msdPortCtrl.c Bonsai_msdEcid.c Bonsai_msdPortPrioMap.c Bonsai_msdQueueCtrl.c Bonsai_msdSysCtrl.c \
	   Bonsai_msdPhyCtrl.c Bonsai_msdRMU.c Bonsai_msdTCAMIntf.c Bonsai_msdQci.c Bonsai_msdQciIntf.c\
	   Bonsai_msdBrgFdbIntf.c Bonsai_msdBrgVtuIntf.c Bonsai_msdBrgStuIntf.c Bonsai_msdPIRLIntf.c Bonsai_msdQav.c \
	   Bonsai_msdArpIntf.c Bonsai_msdPortRmonIntf.c Bonsai_msdPortCtrlIntf.c Bonsai_msdPortPrioMapIntf.c \
	   Bonsai_msdEcidIntf.c Bonsai_msdQueueCtrlIntf.c Bonsai_msdSysCtrlIntf.c Bonsai_msdPhyCtrlIntf.c Bonsai_msdPolicyIntf.c \
	   Bonsai_msdTCAMRCIntf.c Bonsai_msdRMUIntf.c Bonsai_msdPTP.c Bonsai_msdPTPIntf.c \
	   Bonsai_msdLed.c Bonsai_msdLedIntf.c Bonsai_msdTCAMRCCompIntf.c Bonsai_msdEEPROM.c Bonsai_msdEEPROMIntf.c

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
OUT_DIR = $(TEMP_DIR)/dev/bonsai/src/api

# Include common build rules
ifeq ($(OS_RUN),LINUX)
include $(MSD_TOOL_DIR)/makelnx.rules
endif

# end of file
