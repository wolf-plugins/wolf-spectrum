// SPDX-FileCopyrightText: © 2023 Patrick Desaulniers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "DistrhoPlugin.hpp"
#include "readerwriterqueue.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "WolfSpectrumParameters.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

class WolfSpectrumPlugin : public Plugin
{
    friend class Spectrogram;

public:
    WolfSpectrumPlugin();
    ~WolfSpectrumPlugin();

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

    enum BlockSize
    {
        BlockSize64 = 0,
        BlockSize128,
        BlockSize256,
        BlockSize512,
        BlockSize1024,
        BlockSize2048,
        BlockSize4096,
        BlockSize8192,
        BlockSize16384,
        BlockSizeCount
    };

    enum ChannelMix
    {
        ChannelMixLRMean = 0,
        ChannelMixL,
        ChannelMixR,
        ChannelMixCount
    };

    enum PeakFall
    {
        PeakFallNormal = 0,
        PeakFallInstant,
        PeakFallCount
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

    moodycamel::ReaderWriterQueue<float> fRingbuffer;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WolfSpectrumPlugin)
};

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
