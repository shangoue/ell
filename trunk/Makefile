# MOLDUR: modular, env-free build system using GNU make
# by Samuel Hangouët

TARGET_FILES = $(wildcard */*.target *.target)

# You can adjust your compiler and options in this file:
include Script/prefix.mk

all: $(TARGET_FILES)

.PHONY: $(TARGET_FILES)

$(TARGET_FILES):
	@echo Processing $@...
	@$(MAKE) --no-print-directory -f $@ COMPILER="${COMPILER}"

clean:
	rm -rf $(BUILD_FOOTPRINT)

test: libELL/test.target XmlParser/xml_test.target
	$(BUILD_FOOTPRINT)/libell_test
	$(BUILD_FOOTPRINT)/xml_test

cleanall:
	rm -rf Build

package:
	./Script/make_package.sh
