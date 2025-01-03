// SPDX-FileCopyrightText: © 2023 Patrick Desaulniers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SPECTROGRAM_HPP_INCLUDED
#define SPECTROGRAM_HPP_INCLUDED

#include "NanoVG.hpp"
#include "PeakFallSmooth.hpp"
#include "Ringbuffer.hpp"
#include "ScrollingTexture.hpp"
#include "StatusBar.hpp"
#include "WolfSpectrumPlugin.hpp"
#include "kiss_fft.h"

START_NAMESPACE_DISTRHO

class Spectrogram;

static constexpr float SPECTROGRAM_MIN_FREQ = 20.f;

class SpectrogramRulers : public NanoSubWidget
{
public:
    SpectrogramRulers(Spectrogram *parent);
    void drawLinearScaleGrid();
    void drawLogScaleGrid();
    void drawBackground();

protected:
    void onNanoDisplay() override;

private:
    Spectrogram *fParent;
};

class Spectrogram : public NanoSubWidget
{
    friend class SpectrogramRulers;

public:
    Spectrogram(UI *ui, Widget *widget, Size<uint> size);
    ~Spectrogram();

    void setParameterValue(uint32_t i, float v);
    void setLogFrequencyScaling(bool yesno);
    void setBlockSize(int blockSize);
    void setHorizontalScrolling(bool yesno);
    void setSampleRate(const double sampleRate);
    void setGridVisibility(bool visible);
    void setChannelMix(const int channelMix);
    void setPeakFall(const int peakFall);
    void setThreshold(const float threshold);

    void update();

    void clear();

protected:
    void onResize(const ResizeEvent &ev) override;
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;
    void onNanoDisplay() override;

private:
    static constexpr int MAX_BLOCK_SIZE = 16384;

    void process();
    void repositionRulers();
    void render();
    void updateCoeffs();
    float getPowerSpectrumdB(const kiss_fft_cpx *out, const int index, const int transformSize);
    void updateFrequencyText();

    //Call this after changing the block size
    void updateFFTConfig();

    UI *fUI;
    wolf::Ringbuffer<float> fSamples;
    PeakFallSmooth fBins[MAX_BLOCK_SIZE / 2];
    bool fLogFrequencyScaling;
    kiss_fft_cfg fFFTConfig;
    ScrollingTexture fScrollingTexture;
    int fBlockSize;
    bool fHorizontalScrolling;
    double fSampleRate;
    bool fMustShowGrid;
    int fChannelMix;
    int fPeakFall;
    float fThreshold;
    bool fMouseDown = false;
    Point<double> fMousePos;
    kiss_fft_cpx fFFTIn[MAX_BLOCK_SIZE];
    kiss_fft_cpx fFFTOut[MAX_BLOCK_SIZE];

    SpectrogramRulers fRulers;
    StatusBar fStatusBar;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spectrogram)
};

END_NAMESPACE_DISTRHO

#endif
