#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#
# Modified by Patrick Desaulniers
#
# --------------------------------------------------------------
# Project name, used for binaries
#
# Set PLUGIN_NAME in Makefile.mk at the project's root directory
#

NAME = $(PLUGIN_NAME)

# --------------------------------------------------------------
# Files to build

OBJS_DSP = \
	Common/Utils/src/Mathf.cpp.o \
	WolfSpectrumPlugin.cpp.o

OBJS_UI  = \
	Common/Utils/src/Mathf.cpp.o \
	Common/Structures/src/Margin.cpp.o \
	Common/Structures/src/Animation.cpp.o \
	Config/src/Config.cpp.o \
	Libs/inih/ini.c.o \
	Libs/inih/INIReader.cpp.o \
	Resources/Fonts/roboto_light.cpp.o \
	Resources/Fonts/chivo_bold.cpp.o \
	Resources/Fonts/chivo_italic.cpp.o \
	Common/Widgets/src/NanoLabel.cpp.o \
	Common/Widgets/src/ResizeHandle.cpp.o \
	Common/Widgets/src/RightClickMenu.cpp.o \
	ScrollingTexture.cpp.o \
	Spectrogram.cpp.o \
	Libs/kissfft/kiss_fft.cpp.o \
	WolfSpectrumUI.cpp.o 

# --------------------------------------------------------------
# Plugins types to build

BUILD_LV2 ?= true
BUILD_VST2 ?= true
BUILD_JACK ?= true
BUILD_DSSI ?= false
BUILD_LADSPA ?= false

# --------------------------------------------------------------
# Do some magic

include ../Makefile.mk

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(BUILD_JACK),true)
ifeq ($(HAVE_JACK),true)
TARGETS += jack
endif
endif

ifeq ($(BUILD_LADSPA),true)
TARGETS += ladspa
endif

ifeq ($(BUILD_DSSI),true)
ifeq ($(HAVE_DGL),true)
ifeq ($(HAVE_LIBLO),true)
TARGETS += dssi
endif
endif
endif

ifeq ($(BUILD_LV2),true)
TARGETS += lv2
endif

ifeq ($(BUILD_VST2),true)
TARGETS += vst
endif

all: $(TARGETS)

# --------------------------------------------------------------
