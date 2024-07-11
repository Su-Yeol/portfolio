# Source files in this directory
TARGET = 
CSOURCES    = Agate_msdHwAccess.c 
ASOURCES    =

# Include common variable definitions
ifeq ($(OS_RUN),LINUX)
include $(MSD_TOOL_DIR)/makelnx.defs
endif

# Add in extra stuffs
EXTRA_INCLUDE    +=
EXTRA_DEFINE    +=
ADDED_CFLAGS    +=

#Output directory
OUT_DIR = $(TEMP_DIR)/dev/agate/src/driver

# Include common build rules
ifeq ($(OS_RUN),LINUX)
include $(MSD_TOOL_DIR)/makelnx.rules
endif

# end of file
