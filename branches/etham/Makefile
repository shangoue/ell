TARGET_FILES = $(wildcard */*.target)

all: $(TARGET_FILES)

.PHONY: $(TARGET_FILES)

$(TARGET_FILES):
	@$(MAKE) --no-print-directory -f $@ MOLDUR_MAKEFILE=$@
