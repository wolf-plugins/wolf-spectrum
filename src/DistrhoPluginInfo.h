// SPDX-FileCopyrightText: © 2023 Patrick Desaulniers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME "Wolf Spectrum"
#define DISTRHO_PLUGIN_BRAND "Wolf Plugins"
#define DISTRHO_PLUGIN_URI "https://github.com/pdesaulniers/wolf-spectrum"
#define DISTRHO_PLUGIN_CLAP_ID "com.github.wolf-plugins.wolf-spectrum"

#define DISTRHO_PLUGIN_NUM_INPUTS 2
#define DISTRHO_PLUGIN_NUM_OUTPUTS 2
#define DISTRHO_PLUGIN_WANT_PROGRAMS 0
#define DISTRHO_PLUGIN_USES_MODGUI 0
#define DISTRHO_PLUGIN_WANT_STATE 0
#define DISTRHO_PLUGIN_WANT_FULL_STATE 0
#define DISTRHO_PLUGIN_WANT_DIRECT_ACCESS 1
#define DISTRHO_PLUGIN_IS_RT_SAFE 1

#define DISTRHO_PLUGIN_HAS_UI 1
#define DISTRHO_UI_USE_NANOVG 1
#define DISTRHO_UI_USER_RESIZABLE 1

#define DISTRHO_PLUGIN_LV2_CATEGORY "lv2:AnalyserPlugin"
#define DISTRHO_PLUGIN_VST3_CATEGORIES "Fx|Analyzer|Stereo"
#define DISTRHO_PLUGIN_CLAP_FEATURES "audio-effect", "analyzer", "stereo"

#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED
