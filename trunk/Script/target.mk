ARCH = $(shell uname -m)
OS = $(shell uname -o | sed "s/[^A-Za-z0-9]/_/g")

ifneq ($(ARCH),x86_64)
ARCH = x86
else
CFLAGS += -fPIC
endif

ifeq ($(MODE),Release)
CFLAGS += -O3
else
MODE = Debug
CFLAGS += -g -O0 -fno-inline
endif

BUILD_FOOTPRINT = $(OS)/$(ARCH)/$(MODE)

ifeq ($(MOLDUR_BUILD_TARGET),)
# prevent environment variable interferences
TARGET=
MAKEFILES=
EXT=
BUILD_DIR=
CFLAGS=
LDFLAGS=
PCH=
CLEAN_MORE=
DEPENDS=

# fork
.PHONY: all
all:
	@$(MAKE) MOLDUR_BUILD_TARGET=1 --no-print-directory -rR -f $(MOLDUR_MAKEFILE)
else
LDFLAGS += -L$(BUILD_FOOTPRINT)

ifeq ($(COMPILER),icc)
CFLAGS += -Wbrief
else
COMPILER = LANG=C g++
CFLAGS += -Winvalid-pch -Wall -pipe -Wno-parentheses
endif

ifeq ($(NOPCH),)
PCH = CommonHeaders.h
CFLAGS += -include $(PCH)
endif

BUILD_DIR = $(BUILD_FOOTPRINT)/$(MODULE)

TARGET := $(addprefix $(BUILD_FOOTPRINT)/,$(TARGET))
DEPENDS := $(addprefix $(BUILD_FOOTPRINT)/,$(DEPENDS))
TARGET_FILES := $(wildcard $(TARGET_FILES))

include Script/system.mk
endif
