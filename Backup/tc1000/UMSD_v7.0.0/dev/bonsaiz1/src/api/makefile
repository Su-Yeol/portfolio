# Source files in this directory
TARGET = 
CSOURCES = BonsaiZ1_msdTCAM.c BonsaiZ1_msdTCAMRC.c BonsaiZ1_msdTCAMRCComp.c BonsaiZ1_msdBrgFdb.c \
	   BonsaiZ1_msdPolicy.c BonsaiZ1_msdBrgVtu.c BonsaiZ1_msdArp.c BonsaiZ1_msdBrgStu.c BonsaiZ1_msdPIRL.c BonsaiZ1_msdPortRmon.c \
	   BonsaiZ1_msdPortCtrl.c BonsaiZ1_msdEcid.c BonsaiZ1_msdPortPrioMap.c BonsaiZ1_msdQueueCtrl.c BonsaiZ1_msdSysCtrl.c \
	   BonsaiZ1_msdPhyCtrl.c BonsaiZ1_msdRMU.c BonsaiZ1_msdTCAMIntf.c \
	   BonsaiZ1_msdBrgFdbIntf.c BonsaiZ1_msdBrgVtuIntf.c BonsaiZ1_msdBrgStuIntf.c BonsaiZ1_msdPIRLIntf.c BonsaiZ1_msdQav.c \
	   BonsaiZ1_msdArpIntf.c BonsaiZ1_msdPortRmonIntf.c BonsaiZ1_msdPortCtrlIntf.c BonsaiZ1_msdPortPrioMapIntf.c \
	   BonsaiZ1_msdEcidIntf.c BonsaiZ1_msdQueueCtrlIntf.c BonsaiZ1_msdSysCtrlIntf.c BonsaiZ1_msdPhyCtrlIntf.c BonsaiZ1_msdPolicyIntf.c \
	   BonsaiZ1_msdTCAMRCIntf.c BonsaiZ1_msdRMUIntf.c BonsaiZ1_msdPTP.c BonsaiZ1_msdPTPIntf.c \
	   BonsaiZ1_msdLed.c BonsaiZ1_msdLedIntf.c BonsaiZ1_msdTCAMRCCompIntf.c BonsaiZ1_msdEEPROM.c BonsaiZ1_msdEEPROMIntf.c

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
OUT_DIR = $(TEMP_DIR)/dev/bonsaiz1/src/api

# Include common build rules
ifeq ($(OS_RUN),LINUX)
include $(MSD_TOOL_DIR)/makelnx.rules
endif

# end of file
