# Source files in this directory
TARGET = 
CSOURCES = Fir_msdTCAM.c Fir_msdTCAMRC.c Fir_msdTCAMRCComp.c Fir_msdBrgFdb.c\
	   Fir_msdPolicy.c Fir_msdBrgVtu.c Fir_msdArp.c Fir_msdBrgStu.c Fir_msdPIRL.c Fir_msdPortRmon.c \
	   Fir_msdPortCtrl.c Fir_msdEcid.c Fir_msdPortPrioMap.c Fir_msdQueueCtrl.c Fir_msdSysCtrl.c \
	   Fir_msdPhyCtrl.c Fir_msdRMU.c Fir_msdTCAMIntf.c Fir_msdFRER.c Fir_msdFRERIntf.c \
	   Fir_msdBrgFdbIntf.c Fir_msdBrgVtuIntf.c Fir_msdBrgStuIntf.c Fir_msdPIRLIntf.c Fir_msdQav.c \
	   Fir_msdArpIntf.c Fir_msdPortRmonIntf.c Fir_msdPortCtrlIntf.c Fir_msdPortPrioMapIntf.c \
	   Fir_msdEcidIntf.c Fir_msdQueueCtrlIntf.c Fir_msdSysCtrlIntf.c Fir_msdPhyCtrlIntf.c Fir_msdPolicyIntf.c \
	   Fir_msdTCAMRCIntf.c Fir_msdRMUIntf.c Fir_msdPTP.c Fir_msdPTPIntf.c \
	   Fir_msdLed.c Fir_msdLedIntf.c Fir_msdTCAMRCCompIntf.c Fir_msdEEPROM.c Fir_msdEEPROMIntf.c \
	   Fir_msdQcr.c Fir_msdQci.c Fir_msdQciIntf.c Fir_msdMACSec.c Fir_msdMACSecIntf.c

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
OUT_DIR = $(TEMP_DIR)/dev/fir/src/api

# Include common build rules
ifeq ($(OS_RUN),LINUX)
include $(MSD_TOOL_DIR)/makelnx.rules
endif

# end of file
