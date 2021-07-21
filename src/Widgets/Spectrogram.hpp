#ifndef SPECTROGRAM_HPP_INCLUDED
#define SPECTROGRAM_HPP_INCLUDED

#include "DistrhoPluginInfo.h"
#include "NanoVG.hpp"
#include "PeakFallSmooth.hpp"
#include "Ringbuffer.hpp"
#include "ScrollingTexture.hpp"
#include "WolfSpectrumPlugin.hpp"
#include "kiss_fft.h"

START_NAMESPACE_DISTRHO

class Spectrogram;

class SpectrogramRulers : public NanoSubWidget
{
public:
    SpectrogramRulers(Spectrogram* parent);
    void drawLinearScaleGrid();
    void drawLogScaleGrid();
    void drawBackground();

protected:
    void onNanoDisplay() override;

private:
    Spectrogram* fParent;
};

class Spectrogram : public NanoSubWidget
{
    friend class SpectrogramRulers;

public:
    Spectrogram(UI* ui, Widget* widget, Size<uint> size);
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

    void clear();

protected:
    void onResize(const ResizeEvent& ev) override;
    void onNanoDisplay() override;

private:
    static constexpr int MAX_BLOCK_SIZE = 16384;
    void process();
    void repositionRulers();
    void draw();
    void updateCoeffs();
    float getPowerSpectrumdB(const kiss_fft_cpx* out, const int index, const int transformSize);

    //Call this after changing the block size
    void updateFFTConfig();

    UI* fUI;
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
    kiss_fft_cpx fFFTIn[MAX_BLOCK_SIZE];
    kiss_fft_cpx fFFTOut[MAX_BLOCK_SIZE];

    SpectrogramRulers fRulers;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spectrogram)
};

END_NAMESPACE_DISTRHO

#endif
