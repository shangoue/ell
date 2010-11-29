# MOLDUR: modular, env-free build system using GNU make
# by Samuel Hangouët

TARGET_FILES = $(wildcard */*.target *.target)

all: $(TARGET_FILES)

.PHONY: $(TARGET_FILES)

$(TARGET_FILES):
	@echo Processing $@...
	@$(MAKE) --no-print-directory -f $@

clean:
	rm -rf Build
