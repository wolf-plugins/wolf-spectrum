# Wolf Spectrum 
[![Build Status](https://travis-ci.org/pdesaulniers/wolf-spectrum.svg?branch=master)](https://travis-ci.org/pdesaulniers/wolf-spectrum)
[![Chat on Matrix](https://matrix.to/img/matrix-badge.svg)](https://riot.im/app/#/room/#wolf-plugins:matrix.org)

![Wolf Spectrum](https://raw.githubusercontent.com/pdesaulniers/wolf-spectrum/master/plugins/wolf-spectrum/Screenshot.png)

Wolf Spectrum is a spectrogram plugin. It can be built as an LV2 or VST plugin and as a standalone Jack application.

#### Features:
* Supports both log and linear frequency scaling
* Resizable UI

## Install

You can find some precompiled plugin binaries in the [Releases](https://github.com/pdesaulniers/wolf-spectrum/releases) tab. Some packages are also available for Ubuntu and Arch Linux:

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
git clone --recursive https://github.com/pdesaulniers/wolf-spectrum.git
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
git submodule update
```
You should then be able to build the plugin with the most recent changes.
