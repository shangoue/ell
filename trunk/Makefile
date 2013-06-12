# MOLDUR: modular, env-free build system using GNU make
# by Samuel Hangouët

TARGET_FILES = $(wildcard */*.target *.target)

#COMPILER = icc
#COMPILER = sunCC
COMPILER = LANG=C g++

all: $(TARGET_FILES)

.PHONY: $(TARGET_FILES)

$(TARGET_FILES):
	@echo Processing $@...
	@$(MAKE) --no-print-directory -f $@ COMPILER="${COMPILER}"

clean:
	rm -rf Build
