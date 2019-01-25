#ifndef WOLF_SPECTRUM_PARAMETERS_HPP_INCLUDED
#define WOLF_SPECTRUM_PARAMETERS_HPP_INCLUDED

#include "src/DistrhoDefines.h"

START_NAMESPACE_DISTRHO

enum Parameters
{
    paramFrequencyScaling = 0,
    paramScrollDirection,
    paramBlockSize,
    paramOversamplingRatio,
    paramChannelMix,
    paramPeakFall,
    paramShowCaptions,
    paramShowUIControls,
    paramCount
};

END_NAMESPACE_DISTRHO

#endif