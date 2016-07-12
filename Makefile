#
# Copyright (c) 2016, Fuzhou Rockchip Electronics Co., Ltd.
# All rights reserved.
#
# Author: Frank Wang <frank.wang@rock-chips.com>
# Date  : 15-JUL-2016
#
# GCC cross compiation tool: gcc-arm-none-eabi-4_8-2014q3
#

ifneq ($(wildcard .config),)
include .config
endif

#
# Default values for build configurations
#

# Build verbosity
V		:= 0

# Cross Compile
CROSS_COMPILE	:= ../prebuilts/gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-

# Build architecture
ARCH		:= cortex-m0

# Build platform
PLATFORMS	:= "RK3399M0"
PLAT		:= RK3399M0
HELP_PLATFORMS	:= $(shell echo ${PLATFORMS} | sed 's/ /|/g')

# Build path
BUILD_BASE	:= ./build
BUILD_PLAT	:= ${BUILD_BASE}/${PLAT}

ifeq (${V},0)
	Q=@
	CHECKCODE_ARGS	+= --no-summary --terse
else
	Q=
endif
export Q

# Check the platform which user inputed is supported or not.
ifeq (${PLAT},)
  $(error "Error: Unknown platform. Please use PLAT=<platform name> to specify the platform.")
endif
ifeq ($(findstring ${PLAT},${PLATFORMS}),)
  $(error "Error: Invalid platform. The following platforms are available: ${PLATFORMS}")
else
  $(shell echo "PLAT=${PLAT}" > .config)
endif

# All PHONY definition
.PHONY: all clean distclean ${ARCH} msg_start
all: msg_start ${ARCH}

.SUFFIXES:

INCLUDES		+= -Iinclude/

# NOTE: Add C source files here
C_SOURCES		:= src/startup.c	\
			   src/main.c

# Flags definition
CFLAGS			+= -g
ASFLAGS			+= -g -Wa,--gdwarf-2

ASFLAGS			+= -mcpu=$(ARCH) -mthumb -Wall -ffunction-sections -O3
CFLAGS			+= -mcpu=$(ARCH) -mthumb -Wall -ffunction-sections -O3

LDFLAGS			+= -mcpu=$(ARCH) -mthumb -g -nostartfiles -O3
LDFLAGS			+= -Wl,--gc-sections
LOCAL_CFLAGS            += -DCPU_FREQ=150

CC			:= ${CROSS_COMPILE}gcc
CPP			:= ${CROSS_COMPILE}cpp
AS			:= ${CROSS_COMPILE}gcc
AR			:= ${CROSS_COMPILE}ar
LD			:= ${CROSS_COMPILE}ld
OC			:= ${CROSS_COMPILE}objcopy
OD			:= ${CROSS_COMPILE}objdump
NM			:= ${CROSS_COMPILE}nm
PP			:= ${CROSS_COMPILE}gcc -E ${CFLAGS}

# Function definition related compilation
define MAKE_C
$(eval OBJ := $(1)/$(patsubst %.c,%.o,$(notdir $(2))))

$(OBJ) : $(2)
	@echo "  CC      $$<"
	$$(Q)$$(CC) $$(CFLAGS) $$(LOCAL_CFLAGS) $$(INCLUDES) -c $$< -o $$@
endef

define MAKE_S
$(eval OBJ := $(1)/$(patsubst %.S,%.o,$(notdir $(2))))

$(OBJ) : $(2)
	@echo "  AS      $$<"
	$$(Q)$$(AS) $$(ASFLAGS) $$(LOCAL_CFLAGS) -c $$< -o $$@
endef

define MAKE_LD
$(1) :
	@echo "  PP      $$<"
	$$(AS) $$(ASFLAGS) -P -E -D__LINKER__ -o $$@ $$<
endef

define MAKE_OBJS
	$(eval C_OBJS := $(filter %.c,$(2)))
	$(eval REMAIN := $(filter-out %.c,$(2)))
	$(eval $(foreach obj,$(C_OBJS),$(call MAKE_C,$(1),$(obj),$(3))))

	$(eval S_OBJS := $(filter %.S,$(REMAIN)))
	$(eval REMAIN := $(filter-out %.S,$(REMAIN)))
	$(eval $(foreach obj,$(S_OBJS),$(call MAKE_S,$(1),$(obj),$(3))))

	$(and $(REMAIN),$(error Unexpected source files present: $(REMAIN)))
endef

# NOTE: The line continuation '\' is required in the next define otherwise we
# end up with a line-feed characer at the end of the last c filename.
# Also bare this issue in mind if extending the list of supported filetypes.
define SOURCES_TO_OBJS
	$(notdir $(patsubst %.c,%.o,$(filter %.c,$(1)))) \
	$(notdir $(patsubst %.S,%.o,$(filter %.S,$(1))))
endef

# NOTE: The Main compilation entry
# MAKE_MCU macro defines the targets and options to build the image.
define MAKE_MCU
	$(eval BUILD_DIR  := ${BUILD_PLAT}/obj)
	$(eval BIN_DIR   := ${BUILD_PLAT}/bin)
	$(eval SOURCES    := $(C_SOURCES))
	$(eval OBJS       := $(addprefix $(BUILD_DIR)/,$(call SOURCES_TO_OBJS,$(SOURCES))))
	$(eval LINKERFILE := $(BUILD_BASE)/arm-gcc-link.ld)
	$(eval MAPFILE    := $(BIN_DIR)/$(PLAT).map)
	$(eval ELF        := $(BIN_DIR)/$(PLAT).elf)
	$(eval DUMP       := $(BIN_DIR)/$(PLAT).dump)
	$(eval BIN        := $(BIN_DIR)/$(PLAT).bin)

	$(eval $(call MAKE_OBJS,$(BUILD_DIR),$(SOURCES),$(1)))

$(BIN_DIR) :
	$$(Q)mkdir -p "$$@"

$(BUILD_DIR) : $(BIN_DIR)
	$$(Q)mkdir -p "$$@"

$(ELF) : $(OBJS)
	@echo "  LD      $$@"
	$$(Q)$$(CC) -o $$@ $$(LDFLAGS) -Wl,-Map=$(MAPFILE) -Wl,-T$(LINKERFILE) \
		$(OBJS)

$(DUMP) : $(ELF)
	@echo "  OD      $$@"
	$${Q}$${OD} -dx $$< > $$@

$(BIN) : $(ELF)
	@echo "  BIN     $$@"
	$$(Q)$$(OC) -O binary $$< $$@
	@echo
	@echo "Built $$@ successfully"
	@echo

.PHONY : ${ARCH}
${ARCH} : $(BUILD_DIR) $(DUMP) $(BIN)
endef

$(eval $(call MAKE_MCU))

# Other common compilation entries
msg_start:
	@echo "  Building ${PLAT}"
	@echo

clean:
	@echo "  CLEAN"
	${Q}rm -rf ${BUILD_BASE}/${PLAT}
	${Q}rm -rf ${VER_BIN_DIR}/$(PLAT)*

distclean:
	@echo "  DISTCLEAN"
	${Q}rm -rf ${BUILD_BASE}/${PLAT}
	${Q}rm -rf ${VER_BIN_DIR}/$(PLAT)*
	${Q}rm -rf .config

help:
	@echo "usage: make PLAT=<${HELP_PLATFORMS}> <all|clean|distclean>"
	@echo ""
	@echo "PLAT is used to specify which platform you wish to build."
	@echo "If no platform is specified in first time, PLAT defaults to: ${DEFAULT_PLAT}"
	@echo ""
	@echo "Supported Targets:"
	@echo "  all            		Build all the project"
	@echo "  clean          		Clean the current platform project"
	@echo "  distclean			Clean the current project and delete .config"
	@echo ""
	@echo "example: build the targets for the RK3399M0 project:"
	@echo "  make PLAT=RK3399M0"
