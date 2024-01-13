###############################################################################
#        Copyright (c)  2009 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary and Confidential 
###############################################################################

_shortname=$(strip $(if $(strip $(filter ECHO%,$(shell echo))),\
              $(shell for %%g in ( "$(1)" ) do echo. %%~fsg),\
              $(shell cmd /Q /C for %g in \( "$(1)" \) do echo. %~fsg)))


MAKE_D:=$(call _shortname,$(BREWMP_PLATFORM)/../make.d)

include $(MAKE_D)/defines.min

# indicate a module must be built and named checkit.dll
BUILD_BREWMODS = checkit

# specifies all the C source files that need to be compiled
checkit_C_SRCS = AEEAppGen \
	AEEModGen

# specifies all the C++ source files that need to be compiled
checkit_CPP_SRCS = checkit

# specifies the cif files to be compiled
checkit_CIFS = checkit

# specify BMP platform include path
# BREWMP_PLATFORM_DIR is internally defined from BREWMP_PLATFORM
INCDIRS := . \
	$(BREWMP_PLATFORM_DIR)/inc \
	$(BREWMP_PLATFORM_DIR)/../inc

ifeq ($(V_TARGET),armgcc)
checkit_C_SRCS +=glibc_stubs
endif

ifneq ($(V_TARGET),Win32)
ifneq ($(V_TARGET),mingw)
checkit_C_SRCS +=mod_malloc
endif
endif


ifeq ($(V),Win32_Debug)
#Win32_Debug package settings
BREWMP_INPUT = Debug/checkit.dll \
	Debug/checkit.mif

endif

ifeq ($(V),Win32_Release)
#Win32_Release package settings
BREWMP_INPUT = Release/checkit.dll \
	Release/checkit.mif

endif

ifneq ($(V_TARGET),Win32)
#Arm package settings
BREWMP_INPUT = $(OBJ_DIR)/checkit.mod \
	$(OBJ_DIR)/checkit.mif

endif

#Win32_Debug settings
ifeq ($(V), Win32_Debug)
DEFINES+=_DEBUG \
	AEE_SIMULATOR
endif

#Win32_Release settings
ifeq ($(V), Win32_Release)
DEFINES+=NDEBUG \
	AEE_SIMULATOR
endif

#ADS12arm9_Debug settings
ifeq ($(V), ADS12arm9_Debug)
DEFINES+=_DEBUG
endif

#ADS12arm9_Release settings
ifeq ($(V), ADS12arm9_Release)
DEFINES+=NDEBUG
endif

#RVCT22arm9_Debug settings
ifeq ($(V), RVCT22arm9_Debug)
DEFINES+=_DEBUG
endif

#RVCT22arm9_Release settings
ifeq ($(V), RVCT22arm9_Release)
DEFINES+=NDEBUG
endif

#RVCT22arm11_Debug settings
ifeq ($(V), RVCT22arm11_Debug)
DEFINES+=_DEBUG
endif

#RVCT22arm11_Release settings
ifeq ($(V), RVCT22arm11_Release)
DEFINES+=NDEBUG
endif

#RVCT30arm9_Debug settings
ifeq ($(V), RVCT30arm9_Debug)
DEFINES+=_DEBUG
endif

#RVCT30arm9_Release settings
ifeq ($(V), RVCT30arm9_Release)
DEFINES+=NDEBUG
endif

#RVCT30arm11_Debug settings
ifeq ($(V), RVCT30arm11_Debug)
DEFINES+=_DEBUG
endif

#RVCT30arm11_Release settings
ifeq ($(V), RVCT30arm11_Release)
DEFINES+=NDEBUG
endif

#armgcc_Debug settings
ifeq ($(V), armgcc_Debug)
DEFINES+=_DEBUG
endif

#armgcc_Release settings
ifeq ($(V), armgcc_Release)
DEFINES+=NDEBUG
endif

# specify paths to search for dependencies
VPATH += . \
	$(BREWMP_PLATFORM_DIR)/system/src \
	$(BREWMP_PLATFORM_DIR)/src

AEEModGen_CC = $(ARMCC)

# Include BMP SDK make.d/rules.min. Do not change this file.
# Should always be the last thing
include $(RULES_MIN)