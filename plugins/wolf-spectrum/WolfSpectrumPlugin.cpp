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

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "WolfSpectrumPlugin.hpp"
#include "Ringbuffer.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

WolfSpectrumPlugin::WolfSpectrumPlugin() : Plugin(paramCount, 0, 0),
										   fRingbuffer(16384 * 2)
{
	parameters[paramFrequencyScaling] = 0.0f;
	parameters[paramScrollDirection] = 0.0f;
	parameters[paramBlockSize] = BlockSize2048;
	parameters[paramShowGrid] = 1.0f;
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
	return d_version(0, 0, 1);
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
	case paramScrollDirection:
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
		break;
	case paramBlockSize:
		parameter.ranges.min = 0;
		parameter.ranges.max = 5;
		parameter.ranges.def = BlockSize2048;
		parameter.hints = kParameterIsAutomable | kParameterIsInteger;
		parameter.name = "Block Size";
		parameter.symbol = "blocksize";
		parameter.enumValues.count = 6;
		parameter.enumValues.restrictedMode = true;
		{
			ParameterEnumerationValue *const values = new ParameterEnumerationValue[parameter.enumValues.count];
			parameter.enumValues.values = values;
			values[0].label = "512 samples";
			values[0].value = BlockSize512;
			values[1].label = "1024 samples";
			values[1].value = BlockSize1024;
			values[2].label = "2048 samples";
			values[2].value = BlockSize2048;
			values[3].label = "4096 samples";
			values[3].value = BlockSize4096;
			values[4].label = "8192 samples";
			values[4].value = BlockSize8192;
			values[5].label = "16384 samples";
			values[5].value = BlockSize16384;
		}
		break;
	case paramShowGrid:
		parameter.ranges.min = 0;
		parameter.ranges.max = 1;
		parameter.ranges.def = 1;
		parameter.hints = kParameterIsAutomable | kParameterIsInteger | kParameterIsBoolean;
		parameter.name = "Show Grid";
		parameter.symbol = "showgrid";
		break;
	}
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
	const MutexLocker csm(fMutex);

	for (uint32_t i = 0; i < frames; ++i)
	{
		fRingbuffer.add(inputs[0][i]);

		outputs[0][i] = inputs[0][i];
		outputs[1][i] = inputs[1][i];
	}
}

Plugin *createPlugin()
{
	return new WolfSpectrumPlugin();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
