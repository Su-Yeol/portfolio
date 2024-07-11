# Source files in this directory
TARGET = 
CSOURCES = msdTCAM.c msdAdvVct.c msdBrgFdb.c msdBrgVtu.c msdBrgStu.c \
		   msdPIRL.c msdPortRmon.c msdPortCtrl.c msdQosMap.c msdQav.c\
		   msdQueueCtrl.c msdSysCtrl.c msdPhyCtrl.c msdIMP.c msdRMU.c \
		   msdTrunk.c msdPTP.c msdLed.c msdArp.c msdEcid.c msdEEPROM.c \
		   msdPolicy.c msdTCAMRC.c msdTCAMRCComp.c msdQci.c msdQcr.c   \
		   msdFRER.c msdMACSec.c

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
OUT_DIR = $(TEMP_DIR)/src/api

# Include common build rules
ifeq ($(OS_RUN),LINUX)
include $(MSD_TOOL_DIR)/makelnx.rules
endif

# end of file
