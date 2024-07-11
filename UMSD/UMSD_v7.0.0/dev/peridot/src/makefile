# makefile - build main object file
#
# modification history
# --------------------
# 05-08-2015,sundk	created from DSDT_3.5
#
#######################################################################

MSD_PROJ_NAME_PKG = $(MSD_PROJ_NAME)_Peridot
TEMP_DIR_PACKAGE = $(TEMP_DIR)/dev/peridot

ifeq ($(OS_RUN),LINUX)
WORK_TO_DO = $(MSD_PROJ_NAME_PKG).o

# Include common variable definitions
include $(MSD_TOOL_DIR)/makelnx.defs
endif

exe : $(WORK_TO_DO)

#.PHONY : exe

vpath %.o    $(OBJDIR)
vpath %.lib  $(OBJDIR)

MAINDRV_PATH = $(MSD_USER_BASE)/dev/peridot/src

SRC_OBJ_DIR = $(MAINDRV_PATH)/driver \
              $(MAINDRV_PATH)/api \

BUILD_DIR += $(TEMP_DIR)/dev/peridot/src/driver \
			 $(TEMP_DIR)/dev/peridot/src/api

OBJECTS_LIST = $(TEMP_DIR)/dev/peridot/src/driver/$(OBJDIR)/driver.o \
               $(TEMP_DIR)/dev/peridot/src/api/$(OBJDIR)/api.o \

OBJECTS = $(OBJECTS_LIST)

OBJDIRS	= $(subst /,\,$(dir $(OBJECTS)))
LIBDIRS	= $(subst /,\,$(TEMP_DIR))

OBJDIRS_/ = $(subst \,/,$(OBJDIRS))
LIBDIRS_/ = $(subst \,/,$(LIBDIRS))

ifeq ($(OS_RUN),LINUX)
$(MSD_PROJ_NAME_PKG).o : $(OBJECTS)
	@ $(ECHO) '------------'
	@ $(ECHO) 'Building $@'
	$(LD) $(LDFLAGS) -Map $ $(TEMP_DIR_PACKAGE)/$(MSD_PROJ_NAME_PKG).map -o $(TEMP_DIR_PACKAGE)/$(MSD_PROJ_NAME_PKG).o $(OBJECTS)

$(OBJECTS) : 
	@mkdir -p $(BUILD_DIR)
	@for i in $(SRC_OBJ_DIR); do	\
		cd $$i; $(MAKE); 	\
	done

endif

FORCE :

.PHONY : clean

ifeq ($(OS_RUN),LINUX)
clean :
	@for i in $(SRC_OBJ_DIR); do	\
		cd $$i; $(RM) -r -f *.o *.map makedeps *obj; 	\
	done
#		cd $$i; $(RM) -f *.o *.map; 	
	@for i in $(OBJDIRS_/); do	\
		$(RM) -r -f  $$i;	\
	done
#		cd $$i..; $(MAKE) clean;	
	@for subdir in $(TEMP_DIR) ; do \
		cd $$subdir; $(RM) -rf *.o *.map dev/peridot; \
	done

endif

#end of file
