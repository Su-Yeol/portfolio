# Source files in this directory
TARGET = 
CSOURCES = Amethyst_msdTCAM.c Amethyst_msdTCAMRC.c Amethyst_msdTCAMRCComp.c Amethyst_msdAdvVct.c Amethyst_msdBrgFdb.c \
	   Amethyst_msdPolicy.c Amethyst_msdBrgVtu.c Amethyst_msdArp.c Amethyst_msdBrgStu.c Amethyst_msdPIRL.c Amethyst_msdPortRmon.c \
	   Amethyst_msdPortCtrl.c Amethyst_msdEcid.c Amethyst_msdPortPrioMap.c Amethyst_msdQueueCtrl.c Amethyst_msdSysCtrl.c \
	   Amethyst_msdPhyCtrl.c Amethyst_msdIMP.c Amethyst_msdRMU.c Amethyst_msdTCAMIntf.c Amethyst_msdAdvVctIntf.c \
	   Amethyst_msdBrgFdbIntf.c Amethyst_msdBrgVtuIntf.c Amethyst_msdBrgStuIntf.c Amethyst_msdPIRLIntf.c Amethyst_msdQav.c \
	   Amethyst_msdArpIntf.c Amethyst_msdPortRmonIntf.c Amethyst_msdPortCtrlIntf.c Amethyst_msdPortPrioMapIntf.c \
	   Amethyst_msdEcidIntf.c Amethyst_msdQueueCtrlIntf.c Amethyst_msdSysCtrlIntf.c Amethyst_msdPhyCtrlIntf.c Amethyst_msdPolicyIntf.c \
	   Amethyst_msdTCAMRCIntf.c Amethyst_msdIMPIntf.c Amethyst_msdRMUIntf.c Amethyst_msdPTP.c Amethyst_msdPTPIntf.c \
	   Amethyst_msdLed.c Amethyst_msdLedIntf.c Amethyst_msdTCAMRCCompIntf.c Amethyst_msdEEPROM.c Amethyst_msdEEPROMIntf.c

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
OUT_DIR = $(TEMP_DIR)/dev/amethyst/src/api

# Include common build rules
ifeq ($(OS_RUN),LINUX)
include $(MSD_TOOL_DIR)/makelnx.rules
endif

# end of file
