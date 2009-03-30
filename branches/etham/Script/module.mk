# config
ROOT = ..
MODULE = $(notdir $(shell pwd))

ARCH = $(shell uname -m)
OS = $(shell uname -o | sed "s/[^A-Za-z0-9]/_/g")

ifneq ($(ARCH),x86_64)
ARCH = x86
else
CFLAGS += -fPIC
endif

MODE = Debug
ifeq ($(MODE),Release)
CFLAGS += -O3
else
MODE = Debug
CFLAGS += -g -O0 -fno-inline
endif

BUILD_FOOTPRINT = $(OS)/$(ARCH)/$(MODE)

COMPILER = LANG=C g++
ifneq ($(filter $(COMPILER),g++),)
CFLAGS += -Winvalid-pch -Wall -pipe -Wno-parentheses
endif

PCH = $(ROOT)/CommonHeaders.h
CFLAGS += -include $(PCH)

BUILD_DIR = $(ROOT)/$(BUILD_FOOTPRINT)/$(MODULE)

ifeq ($(MOLDUR_BUILD_LIB),)
# prevent environment variable interferences
MAKEFILES=
TARGET=
TARGET_FILES=
EXT=
COMPILER=
BUILD_DIR=
CFLAGS=
LDFLAGS=
PCH=
CLEAN_MORE=
DEPENDS=

# fork
.PHONY: all lib other
all: lib other
lib	other:
	@$(MAKE) MOLDUR_BUILD_LIB=$@ --no-print-directory -rR
else

CFLAGS += -IInclude $(patsubst %,-I$(ROOT)/%/Include,$(USED_MODULES))
LDFLAGS += -L$(ROOT)/$(BUILD_FOOTPRINT) $(addprefix -l,$(USED_MODULES))

ifeq ($(MOLDUR_BUILD_LIB),lib)

# build library (even if empty)
TARGET_FILES = $(wildcard Source/*.cpp Source/*/*.cpp)
TARGET = $(ROOT)/$(BUILD_FOOTPRINT)/lib$(MODULE).a

else

# build other target described in makefile
USED_MODULES += $(MODULE)
ifneq ($(TARGET_FILES)$(TARGET),)
TARGET := $(BUILD_DIR)/$(TARGET)
TARGET_FILES := $(wildcard $(TARGET_FILES))
DEPENDS += $(patsubst %,$(ROOT)/$(BUILD_FOOTPRINT)/lib%.a,$(USED_MODULES))
endif

endif
endif

-include $(ROOT)/Script/system.mk
