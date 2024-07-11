# makefile - build main object file
#
# modification history
# --------------------
# 25-05-2016,sundk	created from MSD_Peridot
#
#######################################################################

TARGET = $(MSD_PROJ_NAME)

ifeq ($(OS_RUN),LINUX)
WORK_TO_DO = $(MSD_PROJ_NAME).o

# Include common variable definitions
include $(MSD_TOOL_DIR)/makelnx.defs
endif

ifeq ($(TOPAZ_BUILD), YES)
TOPAZ_CLEAN = 
endif

vpath %.o    $(OBJDIR)
vpath %.lib  $(OBJDIR)

MAINDRV_PATH = $(SRC_BASE_PATH)

SRC_OBJ_DIR = $(MAINDRV_PATH)/driver \
               $(MAINDRV_PATH)/api \
               $(MAINDRV_PATH)/platform \
               $(MAINDRV_PATH)/utils

BUILD_DIR += $(TEMP_DIR)/src \
			 $(TEMP_DIR)/dev \
			 $(TEMP_DIR)/src/driver \
             $(TEMP_DIR)/src/api \
             $(TEMP_DIR)/src/platform \
             $(TEMP_DIR)/src/utils

ifeq ($(TOPAZ_BUILD), YES)
SRC_OBJ_DIR_TOPAZ += $(MSD_USER_BASE)/dev/topaz \
					 $(MSD_USER_BASE)/dev/topaz/src/api \
				     $(MSD_USER_BASE)/dev/topaz/src/driver

BUILD_DIR += $(TEMP_DIR)/dev/topaz \
			 $(TEMP_DIR)/dev/topaz/src/api \
	         $(TEMP_DIR)/dev/topaz/src/driver
			
endif

ifeq ($(PERIDOT_BUILD), YES)
SRC_OBJ_DIR_PERIDOT += $(MSD_USER_BASE)/dev/peridot \
					   $(MSD_USER_BASE)/dev/peridot/src/api \
					   $(MSD_USER_BASE)/dev/peridot/src/driver	

BUILD_DIR += $(TEMP_DIR)/dev/peridot \
			 $(TEMP_DIR)/dev/peridot/src/api \
	         $(TEMP_DIR)/dev/peridot/src/driver
		
endif

ifeq ($(AGATE_BUILD), YES)
SRC_OBJ_DIR_AGATE += $(MSD_USER_BASE)/dev/agate \
					 $(MSD_USER_BASE)/dev/agate/src/api \
					 $(MSD_USER_BASE)/dev/agate/src/driver

BUILD_DIR += $(TEMP_DIR)/dev/agate \
			 $(TEMP_DIR)/dev/agate/src/api \
	         $(TEMP_DIR)/dev/agate/src/driver

endif

ifeq ($(PEARL_BUILD), YES)
SRC_OBJ_DIR_PEARL += $(MSD_USER_BASE)/dev/pearl \
					 $(MSD_USER_BASE)/dev/pearl/src/api \
					 $(MSD_USER_BASE)/dev/pearl/src/driver

BUILD_DIR += $(TEMP_DIR)/dev/pearl \
			 $(TEMP_DIR)/dev/pearl/src/api \
	         $(TEMP_DIR)/dev/pearl/src/driver

endif

ifeq ($(AMETHYST_BUILD), YES)
SRC_OBJ_DIR_AMETHYST += $(MSD_USER_BASE)/dev/amethyst \
						$(MSD_USER_BASE)/dev/amethyst/src/api \
						$(MSD_USER_BASE)/dev/amethyst/src/driver

BUILD_DIR += $(TEMP_DIR)/dev/amethyst \
			 $(TEMP_DIR)/dev/amethyst/src/api \
	         $(TEMP_DIR)/dev/amethyst/src/driver

endif

ifeq ($(OAK_BUILD), YES)
SRC_OBJ_DIR_OAK += $(MSD_USER_BASE)/dev/oak \
						$(MSD_USER_BASE)/dev/oak/src/api \
						$(MSD_USER_BASE)/dev/oak/src/driver

BUILD_DIR += $(TEMP_DIR)/dev/oak \
			 $(TEMP_DIR)/dev/oak/src/api \
	         $(TEMP_DIR)/dev/oak/src/driver

endif

ifeq ($(SPRUCE_BUILD), YES)
SRC_OBJ_DIR_SPRUCE += $(MSD_USER_BASE)/dev/spruce \
						$(MSD_USER_BASE)/dev/spruce/src/api \
						$(MSD_USER_BASE)/dev/spruce/src/driver

BUILD_DIR += $(TEMP_DIR)/dev/spruce \
			 $(TEMP_DIR)/dev/spruce/src/api \
	         $(TEMP_DIR)/dev/spruce/src/driver

endif			   

ifeq ($(BONSAIZ1_BUILD), YES)
SRC_OBJ_DIR_BONSAIZ1 += $(MSD_USER_BASE)/dev/bonsaiz1 \
						$(MSD_USER_BASE)/dev/bonsaiz1/src/api \
						$(MSD_USER_BASE)/dev/bonsaiz1/src/driver

BUILD_DIR += $(TEMP_DIR)/dev/bonsaiz1 \
			 $(TEMP_DIR)/dev/bonsaiz1/src/api \
	         $(TEMP_DIR)/dev/bonsaiz1/src/driver

endif

ifeq ($(FIR_BUILD), YES)
SRC_OBJ_DIR_FIR += $(MSD_USER_BASE)/dev/fir \
						$(MSD_USER_BASE)/dev/fir/src/api \
						$(MSD_USER_BASE)/dev/fir/src/driver

BUILD_DIR += $(TEMP_DIR)/dev/fir \
			 $(TEMP_DIR)/dev/fir/src/api \
	         $(TEMP_DIR)/dev/fir/src/driver

endif

ifeq ($(BONSAI_BUILD), YES)
SRC_OBJ_DIR_BONSAI += $(MSD_USER_BASE)/dev/bonsai \
						$(MSD_USER_BASE)/dev/bonsai/src/api \
						$(MSD_USER_BASE)/dev/bonsai/src/driver

BUILD_DIR += $(TEMP_DIR)/dev/bonsai \
			 $(TEMP_DIR)/dev/bonsai/src/api \
	         $(TEMP_DIR)/dev/bonsai/src/driver

endif


OBJECTS_LIST = $(TEMP_DIR)/src/driver/$(OBJDIR)/driver.o \
               $(TEMP_DIR)/src/api/$(OBJDIR)/api.o \
               $(TEMP_DIR)/src/platform/$(OBJDIR)/platform.o \
               $(TEMP_DIR)/src/utils/$(OBJDIR)/utils.o

OBJECTS = $(OBJECTS_LIST)

OBJDIRS	= $(subst /,\,$(dir $(OBJECTS)))
LIBDIRS	= $(subst /,\,$(TEMP_DIR))

OBJDIRS_/ = $(subst \,/,$(OBJDIRS))
LIBDIRS_/ = $(subst \,/,$(LIBDIRS))

ifeq ($(OS_RUN),LINUX)
$(MSD_PROJ_NAME).o : $(OBJECTS) $(PACKAGE_OBJECT_TOPAZ) $(PACKAGE_OBJECT_PERIDOT) $(PACKAGE_OBJECT_AGATE) $(PACKAGE_OBJECT_PEARL) $(PACKAGE_OBJECT_AMETHYST) $(PACKAGE_OBJECT_OAK) $(PACKAGE_OBJECT_SPRUCE) $(PACKAGE_OBJECT_BONSAIZ1) $(PACKAGE_OBJECT_FIR) $(PACKAGE_OBJECT_BONSAI)
	@ $(ECHO) '------------'
	@ $(ECHO) 'Building $@'
	$(LD) $(LDFLAGS) -Map $(LIB_DIR)/$(MSD_PROJ_NAME).map -o $(LIB_DIR)/$(MSD_PROJ_NAME).o $(OBJECTS) $(PACKAGE_OBJECT_TOPAZ) $(PACKAGE_OBJECT_PERIDOT) $(PACKAGE_OBJECT_AGATE) $(PACKAGE_OBJECT_PEARL) $(PACKAGE_OBJECT_AMETHYST) $(PACKAGE_OBJECT_OAK) $(PACKAGE_OBJECT_SPRUCE) $(PACKAGE_OBJECT_BONSAIZ1) $(PACKAGE_OBJECT_FIR) $(PACKAGE_OBJECT_BONSAI)

$(OBJECTS) : 
	@mkdir -p $(BUILD_DIR)
	@for i in $(SRC_OBJ_DIR); do \
		cd $$i; $(MAKE) ; \
	done

$(PACKAGE_OBJECT_TOPAZ): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/topaz/src;

$(PACKAGE_OBJECT_PERIDOT): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/peridot/src;

$(PACKAGE_OBJECT_AGATE): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/agate/src;

$(PACKAGE_OBJECT_PEARL): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/pearl/src;
	
$(PACKAGE_OBJECT_AMETHYST): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/amethyst/src;

$(PACKAGE_OBJECT_OAK): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/oak/src;

$(PACKAGE_OBJECT_SPRUCE): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/spruce/src;
    
$(PACKAGE_OBJECT_BONSAIZ1): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/bonsaiz1/src;

$(PACKAGE_OBJECT_FIR): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/fir/src;

$(PACKAGE_OBJECT_BONSAI): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/bonsai/src;
	
endif

FORCE :

.PHONY : clean

ifeq ($(OS_RUN),LINUX)
clean : 
	@for i in $(SRC_OBJ_DIR); do	\
		cd $$i; $(RM) -r -f *.o *.map makedeps *obj; \
	done
	
	-@rm -rf $(SW_ROOT)/obj/
	
	@for i in $(LIB_DIR); do	\
		cd $$i; $(RM) -r -f *.o *.map makedeps *obj; \
	done

endif

#end of file
