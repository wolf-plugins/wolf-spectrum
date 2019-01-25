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
	Common/Structures/src/Oversampler.cpp.o \
	Libs/DSPFilters/source/Butterworth.cpp.o \
	Libs/DSPFilters/source/Biquad.cpp.o \
	Libs/DSPFilters/source/Cascade.cpp.o \
	Libs/DSPFilters/source/Filter.cpp.o \
	Libs/DSPFilters/source/Param.cpp.o \
	Libs/DSPFilters/source/Butterworth.cpp.o \
	Libs/DSPFilters/source/PoleFilter.cpp.o \
	Libs/DSPFilters/source/RBJ.cpp.o \
	Libs/DSPFilters/source/Bessel.cpp.o \
	Libs/DSPFilters/source/RootFinder.cpp.o \
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
	Libs/kissfft/kiss_fft.c.o \
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
