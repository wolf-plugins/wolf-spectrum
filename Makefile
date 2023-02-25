#!/usr/bin/make -f

# SPDX-FileCopyrightText: © Patrick Desaulniers
# SPDX-License-Identifier: GPL-3.0-or-later

export DISTRHO_NAMESPACE = WolfSpectrumDISTRHO
export DGL_NAMESPACE = WolfSpectrumDGL
export FILE_BROWSER_DISABLED = true

include dpf/Makefile.base.mk

all: libs plugins gen

# --------------------------------------------------------------

libs:
	$(MAKE) -C dpf/dgl opengl

plugins: libs
	$(MAKE) all -C src

ifneq ($(CROSS_COMPILING),true)
gen: plugins dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh
ifeq ($(MACOS),true)
	@$(CURDIR)/dpf/utils/generate-vst-bundles.sh
endif

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen: plugins dpf/utils/lv2_ttl_generator.exe
	@$(CURDIR)/dpf/utils/generate-ttl.sh

dpf/utils/lv2_ttl_generator.exe:
	$(MAKE) -C dpf/utils/lv2-ttl-generator WINDOWS=true
endif

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C dpf/dgl
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(MAKE) clean -C src
	rm -rf bin build

install: all
	$(MAKE) install -C src

install-user: all
	$(MAKE) install-user -C src

# --------------------------------------------------------------

.PHONY: all clean install install-user libs plugins gen
