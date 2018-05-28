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

#include "WolfSpectrumParameters.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

class WolfSpectrum : public Plugin
{
  public:
	WolfSpectrum() : Plugin(paramCount, 0, 0)
	{
		
	}

  protected:
	const char *getLabel() const noexcept override
	{
		return "Wolf Spectrum";
	}

	const char *getDescription() const noexcept override
	{
		return "Spectrogram plugin.";
	}

	const char *getMaker() const noexcept override
	{
		return "Patrick Desaulniers";
	}

	const char *getHomePage() const noexcept override
	{
		return "https://github.com/pdesaulniers/wolf-spectrum";
	}

	const char *getLicense() const noexcept override
	{
		return "GPL v3+";
	}

	uint32_t getVersion() const noexcept override
	{
		return d_version(0, 0, 1);
	}

	int64_t getUniqueId() const noexcept override
	{
		return d_cconst('w', 'S', 'c', 'g');
	}

	void initParameter(uint32_t index, Parameter &parameter) override
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

	float getParameterValue(uint32_t index) const override
	{
		return parameters[index];
	}

	void setParameterValue(uint32_t index, float value) override
	{
		parameters[index] = value;
	}

	void run(const float **inputs, float **outputs, uint32_t frames) override
	{		
		for(uint32_t i = 0; i < frames; ++i)
		{
			outputs[0][i] = inputs[0][i];
			outputs[1][i] = inputs[1][i];

			setParameterValue(paramOutLeft, outputs[0][i]);
			setParameterValue(paramOutRight, outputs[1][i]);
		}
	}

  private:
	float parameters[paramCount];

	DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WolfSpectrum)
};

Plugin *createPlugin()
{
	return new WolfSpectrum();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
