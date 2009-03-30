# Simple MaKefile,
# but fullly featured. GNU make probably required
# Samuel Hangouët
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

.PHONY: default clean

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
COL1=$(PF) "\033[34m"
COL2=$(PF) "\033[32m"
COL3=$(PF) "\033[33m"
COLE="\033[0m"
endif

default: $(TARGET)

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
	$(COMPILER) -x $(PCHLANG) $(CFLAGS) -MG -MP -MMD -o $(GCH) -c $<

-include $(DCH)
endif

ifneq ($(TARGET),)
TARGET_OBJ := $(addprefix $(BUILD_DIR)/,$(TARGET_FILES:%$(EXT)=%.o))
TARGET_DEP := $(TARGET_OBJ:%.o=%.d)

ifeq ($(suffix $(TARGET)),.a)
$(TARGET): $(TARGET_OBJ) $(DEPENDS)
	@mkdir -p $(dir $@)
	@echo $(COL1)Create static library $@ $(COLE)
	ar -rucs $@ $(TARGET_OBJ)
else
ifeq ($(suffix $(TARGET)),.so)
$(TARGET): %.so: $(TARGET_OBJ) $(DEPENDS)
	@mkdir -p $(dir $@)
	@echo $(COL1)Create shared library $@ $(COLE)
	$(COMPILER) -Wl,--no-undefined -shared -rdynamic -o $@ $(TARGET_OBJ) $(CFLAGS) $(LDFLAGS)
else
$(TARGET): $(TARGET_OBJ) $(DEPENDS)
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
	$(COMPILER) $(CFLAGS) -MG -MP -MMD -o $(@:%.d=%.o) -c $<

-include $(TARGET_DEP)
endif

ifeq ($(BUILD_DIR),.)
clean:
	@rm -f $(CLEAN_MORE) $(GCH) $(DCH) $(TARGET_DEP) $(TARGET_OBJ) $(TARGET)
else
clean:
	@rm -rf $(TARGET) $(BUILD_DIR)/*
	@if [ -d "$(BUILD_DIR)" ]; then rmdir -p --ignore-fail-on-non-empty $(BUILD_DIR); fi
	@rm -f $(CLEAN_MORE)
endif

%:
	@echo "Don't know how to make $@"
