#----------------------------------------------

# this doesn't really work right now

AUTHOR="Patrick Desaulniers"
YEAR="2018"

read -p "Plugin name: " PLUGIN_NAME
read -p "Plugin class name (Name{Plugin}, Name{UI}): " PLUGIN_CLASS_NAME

PLUGIN_CLASS_NAME="${PLUGIN_CLASS_NAME}Plugin"
PLUGIN_UI_CLASS_NAME="${PLUGIN_CLASS_NAME}UI"

HAS_UI=false

read -p "Does the plugin have an UI (y/n)? " _HAS_UI
if [ "$_HAS_UI" = "y" ]; then
  HAS_UI=true
fi

IS_EFFECT=false

read -p "Effect or synth (e/s)? " _IS_EFFECT
if [ "$_IS_EFFECT" = "e" ]; then
  IS_EFFECT=true
fi

mkdir "$PLUGIN_NAME"
cd "$PLUGIN_NAME"

git init
git submodule add -b enum-parameters git@github.com:pdesaulniers/DPF.git #just change the branch

mkdir bin

echo "All final plugin builds will be placed in this folder.\n\n \
Simply copy those files to their appropriate place." > bin/README

#----------------------------------------------

cat << END > "Makefile"
#----------------------------------------------

#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

include Makefile.mk

all: libs plugins gen

# --------------------------------------------------------------

PREFIX  ?= /usr/local
DESTDIR ?=

# --------------------------------------------------------------

libs:
ifeq (\$(HAVE_DGL),true)
	\$(MAKE) -C dpf/dgl
endif

plugins: libs
	\$(MAKE) all -C plugins/${PLUGIN_NAME}

gen: plugins dpf/utils/lv2_ttl_generator
	@\$(CURDIR)/dpf/utils/generate-ttl.sh
ifeq (\$(MACOS),true)
	@\$(CURDIR)/dpf/utils/generate-vst-bundles.sh
endif

dpf/utils/lv2_ttl_generator:
	\$(MAKE) -C dpf/utils/lv2-ttl-generator

# --------------------------------------------------------------

clean:
ifeq (\$(HAVE_DGL),true)
	\$(MAKE) clean -C dpf/dgl
endif
	\$(MAKE) clean -C dpf/utils/lv2-ttl-generator

	\$(MAKE) clean -C plugins/${PLUGIN_NAME}

# --------------------------------------------------------------

install:
	install -d \$(DESTDIR)\$(PREFIX)/lib/ladspa/
	install -d \$(DESTDIR)\$(PREFIX)/lib/dssi/
	install -d \$(DESTDIR)\$(PREFIX)/lib/lv2/
	install -d \$(DESTDIR)\$(PREFIX)/lib/vst/

	cp bin/*-ladspa.* \$(DESTDIR)\$(PREFIX)/lib/ladspa/
	cp bin/*-dssi.*   \$(DESTDIR)\$(PREFIX)/lib/dssi/
	cp bin/*-vst.*    \$(DESTDIR)\$(PREFIX)/lib/vst/

ifeq (\$(HAVE_DGL),true)
	cp -r bin/*-dssi  \$(DESTDIR)\$(PREFIX)/lib/dssi/
endif
	cp -r bin/*.lv2   \$(DESTDIR)\$(PREFIX)/lib/lv2/

# --------------------------------------------------------------

.PHONY: plugins

END

#----------------------------------------------

cat << END > "Makefile.mk"
#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

AR  ?= ar
CC  ?= gcc
CXX ?= g++

EXTRA_INCLUDES ?=
EXTRA_LIBS ?=

# --------------------------------------------------------------
# Fallback to Linux if no other OS defined

ifneq (\$(HAIKU),true)
ifneq (\$(MACOS),true)
ifneq (\$(WIN32),true)
LINUX=true
endif
endif
endif

# --------------------------------------------------------------
# Set build and link flags

BASE_FLAGS = -Wall -Wextra -pipe
BASE_OPTS  = -O2 -ffast-math -mtune=generic -msse -msse2 -fdata-sections -ffunction-sections

ifneq (\$(MACOS),true)
# MacOS doesn't support this
BASE_OPTS += -mfpmath=sse
endif

ifeq (\$(MACOS),true)
# MacOS linker flags
LINK_OPTS  = -fdata-sections -ffunction-sections -Wl,-dead_strip -Wl,-dead_strip_dylibs
else
# Common linker flags
LINK_OPTS  = -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-O1 -Wl,--as-needed
ifneq (\$(SKIP_STRIPPING),true)
LINK_OPTS += -Wl,--strip-all
endif
endif

ifeq (\$(RASPPI),true)
# Raspberry-Pi optimization flags
BASE_OPTS  = -O2 -ffast-math -march=armv6 -mfpu=vfp -mfloat-abi=hard
LINK_OPTS  = -Wl,-O1 -Wl,--as-needed -Wl,--strip-all
endif

ifeq (\$(PANDORA),true)
# OpenPandora optimization flags
BASE_OPTS  = -O2 -ffast-math -march=armv7-a -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LINK_OPTS  = -Wl,-O1 -Wl,--as-needed -Wl,--strip-all
endif

ifeq (\$(NOOPT),true)
# No optimization flags
BASE_OPTS  = -O2 -ffast-math -fdata-sections -ffunction-sections
endif

ifneq (\$(WIN32),true)
# not needed for Windows
BASE_FLAGS += -fPIC -DPIC
endif

ifeq (\$(DEBUG),true)
BASE_FLAGS += -DDEBUG -O0 -g
LINK_OPTS   =
else
BASE_FLAGS += -DNDEBUG \$(BASE_OPTS) -fvisibility=hidden
CXXFLAGS   += -fvisibility-inlines-hidden
endif

BUILD_C_FLAGS   = \$(BASE_FLAGS) -std=c99 -std=gnu99 \$(CFLAGS) \$(EXTRA_INCLUDES)
BUILD_CXX_FLAGS = \$(BASE_FLAGS) -std=c++0x -std=gnu++0x \$(CXXFLAGS) \$(CPPFLAGS) \$(EXTRA_INCLUDES)
LINK_FLAGS      = \$(LINK_OPTS) -Wl,--no-undefined \$(LDFLAGS) \$(EXTRA_LIBS)

ifeq (\$(MACOS),true)
# No C++11 support
BUILD_CXX_FLAGS = \$(BASE_FLAGS) \$(CXXFLAGS) \$(CPPFLAGS) \$(EXTRA_INCLUDES)
LINK_FLAGS      = \$(LINK_OPTS) \$(LDFLAGS) \$(EXTRA_LIBS)
endif

# --------------------------------------------------------------
# Check for optional libs

ifeq (\$(LINUX),true)
HAVE_DGL   = \$(shell pkg-config --exists gl x11 && echo true)
HAVE_JACK  = \$(shell pkg-config --exists jack   && echo true)
HAVE_LIBLO = \$(shell pkg-config --exists liblo  && echo true)
endif

ifeq (\$(MACOS),true)
HAVE_DGL = true
endif

ifeq (\$(WIN32),true)
HAVE_DGL = true
endif

# --------------------------------------------------------------
# Set libs stuff

ifeq (\$(HAVE_DGL),true)

ifeq (\$(LINUX),true)
DGL_FLAGS = \$(shell pkg-config --cflags gl x11 xcursor) -static-libgcc -static-libstdc++ -lpthread
DGL_LIBS  = \$(shell pkg-config --libs gl x11 xcursor) -static-libgcc -static-libstdc++ -lpthread
endif

ifeq (\$(MACOS),true)
DGL_LIBS  = -framework OpenGL -framework Cocoa
endif

ifeq (\$(WIN32),true)
DGL_LIBS  = -lopengl32 -lgdi32 -static-libgcc -static-libstdc++ -static -lpthread
endif

endif # HAVE_DGL

# --------------------------------------------------------------
# Set app extension

ifeq (\$(WIN32),true)
APP_EXT = .exe
endif

# --------------------------------------------------------------
# Set shared lib extension

LIB_EXT = .so

ifeq (\$(MACOS),true)
LIB_EXT = .dylib
endif

ifeq (\$(WIN32),true)
LIB_EXT = .dll
endif

# --------------------------------------------------------------
# Set shared library CLI arg

SHARED = -shared

ifeq (\$(MACOS),true)
SHARED = -dynamiclib
endif

# --------------------------------------------------------------

END

#----------------------------------------------

mkdir -p "plugins/$PLUGIN_NAME/"
cd "plugins/"

#----------------------------------------------

cat << END > "Makefile.mk"
#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# NAME, OBJS_DSP and OBJS_UI have been defined before

include ../../Makefile.mk

ifeq (\$(OBJS_UI),)
HAVE_DGL = false
endif

# --------------------------------------------------------------
# Basic setup

TARGET_DIR = ../../bin

BUILD_C_FLAGS   += -I.
BUILD_CXX_FLAGS += -I. -I../../dpf/distrho -I../../dpf/dgl

ifeq (\$(HAVE_DGL),true)
BASE_FLAGS += -DHAVE_DGL
endif

ifeq (\$(HAVE_JACK),true)
BASE_FLAGS += -DHAVE_JACK
endif

ifeq (\$(HAVE_LIBLO),true)
BASE_FLAGS += -DHAVE_LIBLO
endif

# --------------------------------------------------------------
# Set plugin binary file targets

jack       = \$(TARGET_DIR)/\$(NAME)\$(APP_EXT)
ladspa_dsp = \$(TARGET_DIR)/\$(NAME)-ladspa\$(LIB_EXT)
dssi_dsp   = \$(TARGET_DIR)/\$(NAME)-dssi\$(LIB_EXT)
dssi_ui    = \$(TARGET_DIR)/\$(NAME)-dssi/\$(NAME)_ui\$(APP_EXT)
lv2        = \$(TARGET_DIR)/\$(NAME).lv2/\$(NAME)\$(LIB_EXT)
lv2_dsp    = \$(TARGET_DIR)/\$(NAME).lv2/\$(NAME)_dsp\$(LIB_EXT)
lv2_ui     = \$(TARGET_DIR)/\$(NAME).lv2/\$(NAME)_ui\$(LIB_EXT)
vst        = \$(TARGET_DIR)/\$(NAME)-vst\$(LIB_EXT)

# --------------------------------------------------------------
# Set distrho code files

DISTRHO_PLUGIN_FILES = ../../dpf/distrho/DistrhoPluginMain.cpp

ifeq (\$(HAVE_DGL),true)
DISTRHO_UI_FILES     = ../../dpf/distrho/src/IdleThread.cpp ../../dpf/distrho/DistrhoUIMain.cpp ../../dpf/libdgl.a
endif

# --------------------------------------------------------------
# Handle plugins without UI

ifneq (\$(HAVE_DGL),true)
dssi_ui =
lv2_ui =
DISTRHO_UI_FILES =
DGL_LIBS =
OBJS_UI =
endif

# --------------------------------------------------------------
# all needs to be first

all:

# --------------------------------------------------------------
# Common

%.c.o: %.c
	\$(CC) $< \$(BUILD_C_FLAGS) -MD -MP -c -o $@ \$(EXTRA_LIBS)

%.cpp.o: %.cpp
	\$(CXX) $< \$(BUILD_CXX_FLAGS) -MD -MP -c -o $@ \$(EXTRA_LIBS)

clean:
	rm -f *.d *.o
	rm -rf \$(TARGET_DIR)/\$(NAME) \$(TARGET_DIR)/\$(NAME)-* \$(TARGET_DIR)/\$(NAME).lv2/

# --------------------------------------------------------------
# JACK

jack: \$(jack)

\$(jack): \$(OBJS_DSP) \$(OBJS_UI) \$(DISTRHO_PLUGIN_FILES) \$(DISTRHO_UI_FILES)
	mkdir -p \$(shell dirname $@)
	\$(CXX) $^ \$(BUILD_CXX_FLAGS) \$(LINK_FLAGS) \$(DGL_LIBS) \$(shell pkg-config --cflags --libs jack) -DDISTRHO_PLUGIN_TARGET_JACK -o $@

# --------------------------------------------------------------
# LADSPA

ladspa: \$(ladspa_dsp)

\$(ladspa_dsp): \$(OBJS_DSP) \$(DISTRHO_PLUGIN_FILES)
	mkdir -p \$(shell dirname $@)
	\$(CXX) $^ \$(BUILD_CXX_FLAGS) \$(LINK_FLAGS) \$(SHARED) -DDISTRHO_PLUGIN_TARGET_LADSPA -o $@

# --------------------------------------------------------------
# DSSI

dssi:     \$(dssi_dsp) \$(dssi_ui)
dssi_dsp: \$(dssi_dsp)
dssi_ui:  \$(dssi_ui)

\$(dssi_dsp): \$(OBJS_DSP) \$(DISTRHO_PLUGIN_FILES)
	mkdir -p \$(shell dirname $@)
	\$(CXX) $^ \$(BUILD_CXX_FLAGS) \$(LINK_FLAGS) \$(SHARED) -DDISTRHO_PLUGIN_TARGET_DSSI -o $@

\$(dssi_ui): \$(OBJS_UI) \$(DISTRHO_UI_FILES)
	mkdir -p \$(shell dirname $@)
	\$(CXX) $^ \$(BUILD_CXX_FLAGS) \$(LINK_FLAGS) \$(DGL_LIBS) \$(shell pkg-config --cflags --libs liblo) -DDISTRHO_PLUGIN_TARGET_DSSI -o $@

# --------------------------------------------------------------
# LV2

lv2_one: \$(lv2)
lv2_dsp: \$(lv2_dsp)
lv2_sep: \$(lv2_dsp) \$(lv2_ui)

\$(lv2): \$(OBJS_DSP) \$(OBJS_UI) \$(DISTRHO_PLUGIN_FILES) \$(DISTRHO_UI_FILES)
	mkdir -p \$(shell dirname $@)
	\$(CXX) $^ \$(BUILD_CXX_FLAGS) \$(LINK_FLAGS) \$(DGL_LIBS) \$(SHARED) -DDISTRHO_PLUGIN_TARGET_LV2 -o $@

\$(lv2_dsp): \$(OBJS_DSP) \$(DISTRHO_PLUGIN_FILES)
	mkdir -p \$(shell dirname $@)
	\$(CXX) $^ \$(BUILD_CXX_FLAGS) \$(LINK_FLAGS) \$(SHARED) -DDISTRHO_PLUGIN_TARGET_LV2 -o $@

\$(lv2_ui): \$(OBJS_UI) \$(DISTRHO_UI_FILES)
	mkdir -p \$(shell dirname $@)
	\$(CXX) $^ \$(BUILD_CXX_FLAGS) \$(LINK_FLAGS) \$(DGL_LIBS) \$(SHARED) -DDISTRHO_PLUGIN_TARGET_LV2 -o $@

# --------------------------------------------------------------
# VST

vst: \$(vst)

\$(vst): \$(OBJS_DSP) \$(OBJS_UI) \$(DISTRHO_PLUGIN_FILES) \$(DISTRHO_UI_FILES)
	mkdir -p \$(shell dirname $@)
	\$(CXX) $^ \$(BUILD_CXX_FLAGS) \$(LINK_FLAGS) \$(DGL_LIBS) \$(SHARED) -DDISTRHO_PLUGIN_TARGET_VST -o $@

# --------------------------------------------------------------

-include \$(OBJS_DSP:%.o=%.d)
ifeq (\$(HAVE_DGL),true)
-include \$(OBJS_UI:%.o=%.d)
endif

# --------------------------------------------------------------

END

#----------------------------------------------

cd "$PLUGIN_NAME"

#----------------------------------------------

cat << END > "Makefile"

#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = ${PLUGIN_NAME}

# --------------------------------------------------------------
# Files to build

OBJS_DSP = \
	${PLUGIN_CLASS_NAME}.cpp.o

OBJS_UI  = \
	${PLUGIN_UI_CLASS_NAME}.cpp.o 

# --------------------------------------------------------------
# Do some magic

include ../Makefile.mk

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq (\$(BUILD_JACK),true)
ifeq (\$(HAVE_JACK),true)
TARGETS += jack
endif
endif

ifeq (\$(BUILD_LADSPA),true)
TARGETS += ladspa
endif

ifeq (\$(BUILD_DSSI),true)
ifeq (\$(HAVE_DGL),true)
ifeq (\$(HAVE_LIBLO),true)
TARGETS += dssi
endif
endif
endif

ifeq (\$(BUILD_LV2),true)
ifeq (\$(HAVE_DGL),true)
TARGETS += lv2_sep
else
TARGETS += lv2_dsp
endif
endif

ifeq (\$(BUILD_VST2),true)
TARGETS += vst
endif

all: \$(TARGETS)

# --------------------------------------------------------------

END

#----------------------------------------------

cat << END > "${PLUGIN_CLASS_NAME}.cpp"
/*
 * ${PLUGIN_NAME}
 * Copyright (C) ${YEAR} ${AUTHOR}
 */

#include "${PLUGIN_CLASS_NAME}.hpp"

START_NAMESPACE_DISTRHO

${PLUGIN_CLASS_NAME}::${PLUGIN_CLASS_NAME}()
    : Plugin(paramCount, 0, 0)
{
    
}

${PLUGIN_CLASS_NAME}::~${PLUGIN_CLASS_NAME}()
{

}

void ${PLUGIN_CLASS_NAME}::initParameter(uint32_t index, Parameter& parameter)
{
    switch (index)
    {

    }
}

float ${PLUGIN_CLASS_NAME}::getParameterValue(uint32_t index) const
{
    return parameters[index];
}

void ${PLUGIN_CLASS_NAME}::setParameterValue(uint32_t index, float value)
{
    parameters[index] = value;
}

void ${PLUGIN_CLASS_NAME}::activate()
{

}

void ${PLUGIN_CLASS_NAME}::deactivate()
{

}

void ${PLUGIN_CLASS_NAME}::run(const float**, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount)
{
    for(int i = 0; i < frames; ++i)
    {

    }
}

Plugin* createPlugin()
{
    return new ${PLUGIN_CLASS_NAME}();
}

END_NAMESPACE_DISTRHO

END

#----------------------------------------------

cat << END > "${PLUGIN_CLASS_NAME}.hpp"
/*
 * ${PLUGIN_NAME}
 * Copyright (C) ${YEAR} ${AUTHOR}
 */

#ifndef ${PLUGIN_CLASS_NAME^^}_HPP_INCLUDED
#define ${PLUGIN_CLASS_NAME^^}_HPP_INCLUDED

START_NAMESPACE_DISTRHO

#include "DistrhoPlugin.hpp"

class ${PLUGIN_CLASS_NAME} : public Plugin
{
public:
    enum Parameters
    {
        paramCount = 0
    };

    ${PLUGIN_CLASS_NAME}();
    ~${PLUGIN_CLASS_NAME}() override;

protected:

    const char* getLabel() const noexcept override
    {
        return "${PLUGIN_NAME}";
    }

    const char* getDescription() const override
    {
		#error Please set the plugin's description.
        return "DPF Plugin.";
    }

    const char* getMaker() const noexcept override
    {
        return "${AUTHOR}";
    }

    const char* getHomePage() const override
    {
		#error Please set the plugin's homepage.
        return "https://github.com/";
    }

    const char* getLicense() const noexcept override
    {
        return "ISC";
    }

    uint32_t getVersion() const noexcept override
    {
        return d_version(0, 0, 1);
    }

    int64_t getUniqueId() const noexcept override
    {			
		#error Please define an id.
		return d_cconst('D', 'm', 'b', 's');
    }

    void initParameter(uint32_t index, Parameter& parameter) override;

    float getParameterValue(uint32_t index) const override;
    void  setParameterValue(uint32_t index, float value) override;

    void activate() override;
    void deactivate() override;
    void run(const float**, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount) override;

private:
    float parameters[paramCount];

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(${PLUGIN_CLASS_NAME})
};

END_NAMESPACE_DISTRHO

#endif  // ${PLUGIN_CLASS_NAME^^}_HPP_INCLUDED

END