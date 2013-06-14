# MOLDUR: modular, env-free build system for unix using GNU make
# by Samuel Hangouët

#COMPILER = icpc
#COMPILER = sunCC
COMPILER = g++
#COMPILER = clang

ARCH = $(shell uname -m)
OS = $(shell uname -o | sed "s/[^A-Za-z0-9]/_/g")

ifneq ($(ARCH),x86_64)
ARCH = x86
else
ifeq ($(findstring sunCC,$(COMPILER)),)
CFLAGS += -fPIC
endif
endif

ifeq ($(MODE),Release)

    CFLAGS += -O3 -DNDEBUG
    LDFLAGS += -s -fno-rtti

else

    MODE = Debug
    CFLAGS += -g -O0

    ifeq ($(findstring sunCC,$(COMPILER)),)
    CFLAGS += -fno-inline
    endif

endif

ifneq ($(findstring icc,$(COMPILER)),)
CFLAGS += -Wbrief
endif

ifneq ($(findstring g++,$(COMPILER)),)
CFLAGS += -Wall -pipe -Wno-parentheses -Wextra
CFLAGS += -Woverloaded-virtual
CFLAGS += -Werror -std=c++11
endif

ifneq ($(findstring clang,$(COMPILER)),)
LDFLAGS += -lstdc++
endif

ifneq ($(findstring sunCC,$(COMPILER)),)
LDFLAGS += -Yl,/usr/bin
endif

BUILD_FOOTPRINT = Build/$(OS)/$(COMPILER)/$(ARCH)/$(MODE)

