/*
 * Wolf Spectrum
 * Copyright (C) 2018 Patrick Desaulniers
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoPlugin.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "Ringbuffer.hpp"
#include "WolfSpectrumPlugin.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

WolfSpectrumPlugin::WolfSpectrumPlugin()
    : Plugin(paramCount, 0, 0),
      fRingbuffer(16384)
{
}

WolfSpectrumPlugin::~WolfSpectrumPlugin()
{
}

const char *WolfSpectrumPlugin::getLabel() const noexcept
{
    return "Wolf Spectrum";
}

const char *WolfSpectrumPlugin::getDescription() const noexcept
{
    return "Spectrogram plugin.";
}

const char *WolfSpectrumPlugin::getMaker() const noexcept
{
    return "Patrick Desaulniers";
}

const char *WolfSpectrumPlugin::getHomePage() const noexcept
{
    return "https://github.com/pdesaulniers/wolf-spectrum";
}

const char *WolfSpectrumPlugin::getLicense() const noexcept
{
    return "GPL v3+";
}

uint32_t WolfSpectrumPlugin::getVersion() const noexcept
{
    return d_version(1, 0, 0);
}

int64_t WolfSpectrumPlugin::getUniqueId() const noexcept
{
    return d_cconst('W', 'S', 'p', 't');
}

void WolfSpectrumPlugin::initParameter(uint32_t index, Parameter &parameter)
{
    switch (index)
    {
    case paramFrequencyScaling:
        parameter.ranges.min = 0;
        parameter.ranges.max = 1;
        parameter.ranges.def = 0;
        parameter.hints = kParameterIsAutomable | kParameterIsInteger;
        parameter.name = "Frequency Scaling";
        parameter.symbol = "frequencyscaling";
        parameter.enumValues.count = 2;
        parameter.enumValues.restrictedMode = true;
        {
            ParameterEnumerationValue *const values = new ParameterEnumerationValue[parameter.enumValues.count];
            parameter.enumValues.values = values;
            values[0].label = "Logarithmic";
            values[0].value = FrequencyScalingLogarithmic;
            values[1].label = "Linear";
            values[1].value = FrequencyScalingLinear;
        }
        break;
    /* case paramScrollDirection:
		parameter.ranges.min = 0;
		parameter.ranges.max = 1;
		parameter.ranges.def = 0;
		parameter.hints = kParameterIsAutomable | kParameterIsInteger;
		parameter.name = "Scroll Direction";
		parameter.symbol = "scrolldirection";
		parameter.enumValues.count = 2;
		parameter.enumValues.restrictedMode = true;
		{
			ParameterEnumerationValue *const values = new ParameterEnumerationValue[parameter.enumValues.count];
			parameter.enumValues.values = values;
			values[0].label = "Vertical";
			values[0].value = ScrollDirectionVertical;
			values[1].label = "Horizontal";
			values[1].value = ScrollDirectionHorizontal;
		}
		break; */
    case paramBlockSize:
        parameter.ranges.min = 0;
        parameter.ranges.max = BlockSizeCount - 1;
        parameter.ranges.def = BlockSize4096;
        parameter.hints = kParameterIsAutomable | kParameterIsInteger;
        parameter.name = "Block Size";
        parameter.symbol = "blocksize";
        parameter.enumValues.count = BlockSizeCount;
        parameter.enumValues.restrictedMode = true;
        {
            ParameterEnumerationValue *const values = new ParameterEnumerationValue[parameter.enumValues.count];
            parameter.enumValues.values = values;
            values[0].label = "64 samples";
            values[0].value = BlockSize64;
            values[1].label = "128 samples";
            values[1].value = BlockSize128;
            values[2].label = "256 samples";
            values[2].value = BlockSize256;
            values[3].label = "512 samples";
            values[3].value = BlockSize512;
            values[4].label = "1024 samples";
            values[4].value = BlockSize1024;
            values[5].label = "2048 samples";
            values[5].value = BlockSize2048;
            values[6].label = "4096 samples";
            values[6].value = BlockSize4096;
            values[7].label = "8192 samples";
            values[7].value = BlockSize8192;
            values[8].label = "16384 samples";
            values[8].value = BlockSize16384;
        }
        break;
    case paramChannelMix:
        parameter.ranges.min = 0;
        parameter.ranges.max = ChannelMixCount - 1;
        parameter.ranges.def = ChannelMixLRMean;
        parameter.hints = kParameterIsAutomable | kParameterIsInteger;
        parameter.name = "Channel Mix";
        parameter.symbol = "channelmix";
        parameter.enumValues.count = ChannelMixCount;
        parameter.enumValues.restrictedMode = true;
        {
            ParameterEnumerationValue *const values = new ParameterEnumerationValue[parameter.enumValues.count];
            parameter.enumValues.values = values;
            values[0].label = "Left/Right (mean)";
            values[0].value = ChannelMixLRMean;
            values[1].label = "Left";
            values[1].value = ChannelMixL;
            values[2].label = "Right";
            values[2].value = ChannelMixR;
        }
        break;
    /* case paramPeakFall:
		parameter.ranges.min = 0;
		parameter.ranges.max = PeakFallCount - 1;
		parameter.ranges.def = PeakFallNormal;
		parameter.hints = kParameterIsAutomable | kParameterIsInteger;
		parameter.name = "Peak Fall";
		parameter.symbol = "peakfall";
		parameter.enumValues.count = PeakFallCount;
		parameter.enumValues.restrictedMode = true;
		{
			ParameterEnumerationValue *const values = new ParameterEnumerationValue[parameter.enumValues.count];
			parameter.enumValues.values = values;
			values[0].label = "Normal";
			values[0].value = PeakFallNormal;
			values[1].label = "Instant";
			values[1].value = PeakFallInstant;
		}
		break; */
    case paramGain:
        parameter.ranges.min = -25.0f;
        parameter.ranges.max = 25.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Gain";
        parameter.symbol = "gain";
        break;
    /* case paramThreshold:
		parameter.ranges.min = -90.0f;
		parameter.ranges.max = -0.1f;
		parameter.ranges.def = -90.0f;
		parameter.hints = kParameterIsAutomable;
		parameter.name = "Threshold";
		parameter.symbol = "threshold";
		break; */
    case paramShowCaptions:
        parameter.ranges.min = 0;
        parameter.ranges.max = 1;
        parameter.ranges.def = 1;
        parameter.hints = kParameterIsAutomable | kParameterIsInteger | kParameterIsBoolean;
        parameter.name = "Show Captions";
        parameter.symbol = "showcaptions";
        break;
    case paramShowUIControls:
        parameter.ranges.min = 0;
        parameter.ranges.max = 1;
        parameter.ranges.def = 1;
        parameter.hints = kParameterIsAutomable | kParameterIsInteger | kParameterIsBoolean;
        parameter.name = "Show UI Controls";
        parameter.symbol = "showuicontrols";
        break;
    }

    parameters[index] = parameter.ranges.def;
}

float WolfSpectrumPlugin::getParameterValue(uint32_t index) const
{
    return parameters[index];
}

void WolfSpectrumPlugin::setParameterValue(uint32_t index, float value)
{
    parameters[index] = value;
}

void WolfSpectrumPlugin::run(const float **inputs, float **outputs, uint32_t frames)
{
    const float gaindB = parameters[paramGain];
    const float gainFactor = std::pow(10.0f, gaindB / 20.0f);

    const ChannelMix channelMix = (ChannelMix)std::round(parameters[paramChannelMix]);

    for (uint32_t i = 0; i < frames; ++i)
    {
        const float sampleL = inputs[0][i] * gainFactor;
        const float sampleR = inputs[1][i] * gainFactor;

        float sampleOut = 0.0f;

        switch (channelMix)
        {
        case WolfSpectrumPlugin::ChannelMixLRMean:
            sampleOut = (sampleL + sampleR) / 2.0f;
            break;
        case WolfSpectrumPlugin::ChannelMixL:
            sampleOut = sampleL;
            break;
        case WolfSpectrumPlugin::ChannelMixR:
            sampleOut = sampleR;
            break;
        default:
            return; //¯\_(ツ)_/¯
        }

        fRingbuffer.try_enqueue(sampleOut);

        outputs[0][i] = inputs[0][i];
        outputs[1][i] = inputs[1][i];
    }
}

Plugin *createPlugin()
{
    return new WolfSpectrumPlugin();
}

END_NAMESPACE_DISTRHO
