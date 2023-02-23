/*
 * Wolf Spectrum audio plugin, based on DISTRHO Plugin Framework (DPF)
 *
 * SPDX-License-Identifier: GPL v3+
 *
 * Copyright (C) 2023 Patrick Desaulniers
 */

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