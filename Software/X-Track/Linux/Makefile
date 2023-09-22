#
# Makefile
#
CC ?= gcc
CXX ?= g++

PROJECT_DIR = ${shell pwd}/..

LVGL_DIR_NAME ?= lvgl
LVGL_DIR ?= $(PROJECT_DIR)/Simulator/LVGL.Simulator

WARNINGS_CFG ?= -Wall -Wextra \
				-Wshadow -Wundef -Wmaybe-uninitialized \
				-Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized \
				-Wno-unused-parameter -Wno-missing-field-initializers -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default  \
				-Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated  \
				-Wempty-body -Wshift-negative-value -Wstack-usage=4096 \
				-Wtype-limits -Wsizeof-pointer-memaccess -Wpointer-arith

INCLUDE_CFG = -I$(LVGL_DIR)/ -I$(PROJECT_DIR)/USER/App -I$(PROJECT_DIR)/USER/App/Utils/ArduinoJson/src

CFLAGS ?= 	-O3 -g0 $(INCLUDE_CFG) $(WARNING_CFG) \
			-DLV_COLOR_DEPTH=32 -DLV_COLOR_16_SWAP=0 -DLV_FS_PC_PATH=\"../../../\" \
			-DCONFIG_MAP_EXT_NAME_DEFAULT=\"png\"
CXXFLAGS ?= $(CFLAGS)
LDFLAGS ?= -lm -lstdc++
BIN = xtrack

#Collect the files to compile
MAINSRC = ./main.cpp

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk

CSRCS += $(shell find -L $(LVGL_DIR)/lv_fs_if -name "*.c")
CSRCS += $(shell find -L $(PROJECT_DIR)/USER/App -name "*.c")
CXXSRCS += $(shell find -L $(LVGL_DIR)/HAL -name "*.cpp")

APP_CXXSRCS = $(shell find -L $(PROJECT_DIR)/USER/App -name "*.cpp")
APP_FILTER_OUT += $(shell find -L $(PROJECT_DIR)/USER/App/Utils/ArduinoJson -name "*.cpp")
APP_FILTER_OUT += $(shell find -L $(PROJECT_DIR)/USER/App/Utils/lv_img_png -name "*.cpp")

CXXSRCS += $(filter-out $(APP_FILTER_OUT), $(APP_CXXSRCS))

CSRCS += mouse_cursor_icon.c

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))
CXXOBJS = $(CXXSRCS:.cpp=$(OBJEXT))
MAINOBJ = $(MAINSRC:.cpp=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(CXXSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS) $(CXXOBJS)

## MAINOBJ -> OBJFILES

all: default

%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"

%.o: %.cpp
	@$(CXX)  $(CXXFLAGS) -c $< -o $@
	@echo "CXX $<"
    
default: $(AOBJS) $(COBJS) $(CXXOBJS) $(MAINOBJ)
	$(CC) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(CXXOBJS) $(LDFLAGS)

clean: 
	rm -f $(BIN) $(AOBJS) $(COBJS) $(CXXOBJS) $(MAINOBJ)

