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

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

WolfSpectrumPlugin::WolfSpectrumPlugin() : Plugin(paramCount, 0, 0)
{
	fSamples = (float **)malloc(sizeof(float *) * 2);

	fSamples[0] = (float *)malloc(1024 * sizeof(float));
	fSamples[1] = (float *)malloc(1024 * sizeof(float));
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
	return d_cconst('w', 'S', 'c', 'g');
}

void WolfSpectrumPlugin::initParameter(uint32_t index, Parameter &parameter)
{
	switch (index)
	{
	case paramOutLeft:
		parameter.name = "Out Left";
		parameter.symbol = "outleft";
		parameter.hints = kParameterIsOutput;
		break;
	case paramOutRight:
		parameter.name = "Out Right";
		parameter.symbol = "outright";
		parameter.hints = kParameterIsOutput;
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
		fSamples[0][i] = inputs[0][i];
		fSamples[1][i] = inputs[1][i];

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
