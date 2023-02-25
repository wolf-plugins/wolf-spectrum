<!--
SPDX-FileCopyrightText: © 2023 Patrick Desaulniers
SPDX-License-Identifier: CC-BY-SA-4.0
-->

# Wolf Spectrum [![Build Status](https://img.shields.io/github/actions/workflow/status/wolf-plugins/wolf-spectrum/dpf-makefile-action.yml?branch=master)](https://github.com/wolf-plugins/wolf-spectrum/actions/workflows/dpf-makefile-action.yml)

![Wolf Spectrum](https://raw.githubusercontent.com/wolf-plugins/wolf-spectrum/master/src/Screenshot.png)

Wolf Spectrum is a spectrogram plugin. It can be built as an LV2 or VST plugin and as a standalone Jack application.

#### Features:

* Supports both log and linear frequency scaling
* Resizable UI

## Install

You can find some precompiled plugin binaries in the [Releases](https://github.com/wolf-plugins/wolf-spectrum/releases) tab. Some packages are also available for Ubuntu and Arch Linux:

### Ubuntu

Download the plugin from the [pdesaulniers/wolf PPA](https://launchpad.net/~pdesaulniers/+archive/ubuntu/wolf):

```
sudo apt-add-repository -y ppa:pdesaulniers/wolf
sudo apt-get update
sudo apt-get install wolf-spectrum
```

### Arch Linux

To install the latest release:

```
sudo pacman -S wolf-spectrum
```

A git package is also available [in the AUR](https://aur.archlinux.org/packages/wolf-spectrum-git/). 

## Build manually

First, clone the repo (note the "--recursive" argument):

```
git clone --recursive https://github.com/wolf-plugins/wolf-spectrum.git
cd wolf-spectrum
```

Then:

```
BUILD_VST2=true BUILD_LV2=true BUILD_JACK=true make
```

Prepend WIN32=true or MACOS=true to the command if applicable.

All plugin builds will then be placed in the bin folder. Copy them to their appropriate place so that your plugin host can find them, and you're done :)

## Updating

This project uses git submodules. Thus, to update your local copy of the repo, you need to run the following commands:

```
git pull
git submodule update --init --recursive
```

You should then be able to build the plugin with the most recent changes.

## License

Wolf Spectrum is licensed under GPL-3.0-or-later.

Some files in this repository are licensed under different terms:

- Some code borrowed from DPF and DISTRHO plugins is licensed under ISC.
- Some configuration files are licensed under CC0-1.0.
- This README is licensed under CC-BY-SA-4.0.

See the individual files for their copyright status.

This program also makes use of the following third-party code:

- [DPF](https://github.com/DISTRHO/DPF)
- [KISS FFT](https://github.com/mborgerding/kissfft)
- [readerwriterqueue](https://github.com/cameron314/readerwriterqueue)

Please refer to each individual project for their copyright status.
