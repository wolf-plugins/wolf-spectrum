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

#pragma once

#include "DistrhoPlugin.hpp"
#include "extra/Mutex.hpp"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "WolfSpectrumParameters.hpp"
#include "WolfSpectrumUI.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

class WolfSpectrumPlugin : public Plugin
{
	friend class WolfSpectrumUI;

  public:
	WolfSpectrumPlugin();

	enum ScrollDirection
	{
		ScrollDirectionVertical = 0,
		ScrollDirectionHorizontal
	};

	enum FrequencyScaling
	{
		FrequencyScalingLogarithmic = 0,
		FrequencyScalingLinear
	};

  protected:
	const char *getLabel() const noexcept override;

	const char *getDescription() const noexcept override;

	const char *getMaker() const noexcept override;

	const char *getHomePage() const noexcept override;

	const char *getLicense() const noexcept override;

	uint32_t getVersion() const noexcept override;

	int64_t getUniqueId() const noexcept override;

	void initParameter(uint32_t index, Parameter &parameter) override;

	float getParameterValue(uint32_t index) const override;

	void setParameterValue(uint32_t index, float value) override;

	void run(const float **inputs, float **outputs, uint32_t frames) override;

  private:
	float parameters[paramCount];

	Mutex fMutex;
	float **fSamples;

	DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WolfSpectrumPlugin)
};

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
