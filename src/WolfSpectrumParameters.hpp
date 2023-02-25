// SPDX-FileCopyrightText: © Patrick Desaulniers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WOLF_SPECTRUM_PARAMETERS_HPP_INCLUDED
#define WOLF_SPECTRUM_PARAMETERS_HPP_INCLUDED

#include "src/DistrhoDefines.h"

START_NAMESPACE_DISTRHO

enum Parameters
{
    paramFrequencyScaling = 0,
    paramBlockSize,
    paramChannelMix,
    paramShowCaptions,
    paramShowUIControls,
    paramGain,
    paramCount
};

END_NAMESPACE_DISTRHO

#endif