# MOLDUR: modular, env-free build system for unix using GNU make
# by Samuel Hangouët

ARCH = $(shell uname -m)
OS = $(shell uname -o | sed "s/[^A-Za-z0-9]/_/g")

ifneq ($(ARCH),x86_64)
ARCH = x86
else
CFLAGS += -fPIC
endif

ifeq ($(MODE),Release)
CFLAGS += -O3 -DNDEBUG -s -fno-rtti
else
MODE = Debug
CFLAGS += -g -O0 -fno-inline
endif

BUILD_FOOTPRINT = Build/$(OS)/$(ARCH)/$(MODE)

########################################
ifeq ($(MOLDUR_BUILD_TARGET),)


# prevent environment variable interferences
TARGET=
COMPILER=
MAKEFILES=
EXT=
BUILD_DIR=
CFLAGS=
LDFLAGS=
PCH=
CLEAN_MORE=
DEPENDS=

# fork
moldur_top:
	@$(MAKE) MOLDUR_BUILD_TARGET=1 --no-print-directory -rR -f $(firstword $(MAKEFILE_LIST))

clean:
	@$(MAKE) MOLDUR_BUILD_TARGET=1 --no-print-directory -rR -f $(firstword $(MAKEFILE_LIST)) clean


########################################
else
LDFLAGS += -L$(BUILD_FOOTPRINT)

#COMPILER = LANG=C icc
ifneq ($(findstring icc,$(COMPILER)),)
CFLAGS += -Wbrief
else
COMPILER = LANG=C g++
CFLAGS += -Wall -pipe -Wno-parentheses -Wextra
CFLAGS += -Woverloaded-virtual
CFLAGS += -Werror
endif

CFLAGS += $(if $(PCH),-include $(PCH))

BUILD_DIR = $(BUILD_FOOTPRINT)/$(MODULE)

TARGET := $(addprefix $(BUILD_FOOTPRINT)/,$(TARGET))
DEPENDS := $(wildcard $(DEPENDS))
TARGET_FILES := $(wildcard $(TARGET_FILES))

ifeq ($(RESSOURCE_PATH),)
RESSOURCE_PATH = $(BUILD_FOOTPRINT)
else
RESSOURCE_PATH := $(addprefix $(BUILD_FOOTPRINT)/,$(RESSOURCE_PATH))
endif

LDFLAGS += -Wl,-rpath,$(shell pwd)/$(BUILD_FOOTPRINT)

# BEGIN OF SYSTEM

# MOLDUR: modular, env-free build system using GNU make
# by Samuel Hangouët
#
# Input variables:
#
# TARGET:       pathname of generated file (.a, .so or runable) (eg. "Binary/i686/Release/AITest")
# TARGET_FILES: additional files for TARGET                     (eg. "Test/AI/main.cpp")
# EXT:          source files extension (default taken from
#               first file in TARGET_FILES)                     (eg. ".cpp")
# COMPILER:     compiler (default g++ or gcc according to EXT)  (eg. "icc")
# BUILD_DIR:    prefix for generated files (default .)          (eg. "Binary/i686/Release")
# CFLAGS:       compilation flags (opt)                         (eg. -Iinclude -Wall)
# LDFLAGS:      link flags (opt)                                (eg. -L$(BUILD_DIR) -lm)
# PCH:          precompiled header (opt)                        (eg. Include/CommonHeaders.h)
# CLEAN_MORE:   files to remove appart from BUILD_DIR (opt)     (eg. Test1.bin Test2)
# DEPENDS:      other target dependencies                       (eg. external/libbullet.a)
# INSTALL:      files to be copied into BUILD_DIR (opt)         (eg. doc/LICENSE.TXT)

.PHONY: clean

ifeq ($(BUILD_DIR),)
BUILD_DIR := .
endif

ifeq ($(EXT),)
ifneq ($(TARGET_FILES),)
EXT = $(suffix $(word 1,$(TARGET_FILES)))
else
EXT = .cpp
endif
endif

ifeq ($(COMPILER),)
ifeq ($(EXT),.c)
COMPILER := LANG=C gcc
else
COMPILER := LANG=C g++
endif
endif

ifneq ($(COLORTERM),)
ifneq ($(shell ls -l /bin/sh | grep bash),)
PF = -e
endif
COL1=$(PF) "\033[34;1m"
COL2=$(PF) "\033[32m"
COL3=$(PF) "\033[33m"
COLE="\033[0m"
endif

TARGET_OBJ := $(addprefix $(BUILD_DIR)/,$(TARGET_FILES:%$(EXT)=%.o))
TARGET_DEP := $(TARGET_OBJ:%.o=%.d)

RESSOURCE_DEST = $(addprefix $(RESSOURCE_PATH)/,$(notdir $(RESSOURCES)))

ifneq ($(TARGET),)
$(TARGET): $(DEPENDS) $(TARGET_OBJ) $(RESSOURCE_DEST)
endif

ifneq ($(RESSOURCES),)
$(RESSOURCE_DEST): $(RESSOURCES)
	@mkdir -p $(RESSOURCE_PATH)
	@cp -uv $^ $(RESSOURCE_PATH)
endif

ifneq ($(PCH),)
ifeq ($(PCHLANG),)
ifeq ($(EXT),.c)
PCHLANG = c-header
else
PCHLANG = c++-header
endif
endif

GCH        := $(BUILD_DIR)/$(PCH).gch
DCH        := $(BUILD_DIR)/$(PCH).d
CFLAGS     := $(addprefix -I,$(dir $(GCH))) $(CFLAGS)

$(GCH): $(PCH)
	@echo $(COL2)PCHCOMP - $< $(COLE)
	$(COMPILER) -x $(PCHLANG) $(CFLAGS) -o $@ -c $<

$(DCH): $(PCH)
	@mkdir -p $(dir $@)
	@echo $(COL3)PCHCOMPD - $< $(COLE)
	@rm -f $(GCH)
	$(COMPILER) -x $(PCHLANG) $(CFLAGS) -MP -MMD -o $(GCH) -c $<

-include $(DCH)
endif

ifeq ($(suffix $(TARGET)),.a)
$(TARGET):
	@mkdir -p $(dir $@)
	@echo $(COL1)Create static library $@ $(COLE)
	ar -rucs $@ $(TARGET_OBJ)
	@touch $@
else
ifeq ($(suffix $(TARGET)),.so)
$(TARGET): %.so:
	@mkdir -p $(dir $@)
	@echo $(COL1)Create shared library $@ $(COLE)
	$(COMPILER) -Wl,--no-undefined -shared -rdynamic -o $@ $(TARGET_OBJ) $(CFLAGS) $(LDFLAGS)
else
$(TARGET):
	@mkdir -p $(dir $@)
	@echo $(COL1)LINK - $@ $(COLE)
	$(COMPILER) $(TARGET_OBJ) -o $(TARGET) $(LDFLAGS) $(CFLAGS)
endif
endif

$(TARGET_OBJ): $(BUILD_DIR)/%.o: %$(EXT) $(DCH) $(BUILD_DIR)/%.d
	@echo $(COL2)COMP - $< $(COLE)
	$(COMPILER) $(CFLAGS) $< -c -o $@

$(TARGET_DEP): $(BUILD_DIR)/%.d: %$(EXT) $(DCH)
	@mkdir -p $(dir $@)
	@echo $(COL3)COMPD - $< $(COLE)
	$(COMPILER) $(CFLAGS) -MP -MMD -o $(@:%.d=%.o) -c $<

-include $(TARGET_DEP)

ifeq ($(BUILD_DIR),.)
clean:
	@rm -f $(CLEAN_MORE) $(GCH) $(DCH) $(TARGET_DEP) $(TARGET_OBJ) $(TARGET)
else
clean:
	@rm -rf $(TARGET) $(BUILD_DIR)/*
	@if [ -d "$(BUILD_DIR)" ]; then rmdir -p --ignore-fail-on-non-empty $(BUILD_DIR); fi
	@rm -f $(CLEAN_MORE)
endif

# END OF SYSTEM

$(TARGET_FILES): $(firstword $(MAKEFILE_LIST))

endif
########################################
