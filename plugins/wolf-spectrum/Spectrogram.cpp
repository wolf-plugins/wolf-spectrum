
#include "Spectrogram.hpp"
#include "DistrhoUI.hpp"
#include "Window.hpp"
#include "Mathf.hpp"
#include "Config.hpp"
#include "WolfSpectrumPlugin.hpp"
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <iostream>
#include <fftw3.h>

START_NAMESPACE_DISTRHO

SpectrogramRulers::SpectrogramRulers(Spectrogram *parent) : NanoWidget(parent),
                                                            fParent(parent)
{
}

void SpectrogramRulers::onNanoDisplay()
{
    if (fParent->fMustShowGrid)
    {
        drawBackground();

        if (fParent->fLogFrequencyScaling)
        {
            drawLogScaleGrid();
        }
        else
        {
            drawLinearScaleGrid();
        }

        /* if (fMustShowFrequencyAtMouse)
        {
            drawFrequencyAtMouse();
        } */
    }
}

void SpectrogramRulers::drawBackground()
{
    const int width = getWidth();
    const int height = getHeight();

    beginPath();
    Paint gradient = linearGradient(0, 0, 0, getHeight(), Color(0, 0, 0, 200), Color(0, 0, 0, 0));
    fillPaint(gradient);
    rect(0, 0, width, height);
    fill();
    closePath();
}

Spectrogram::Spectrogram(UI *ui, NanoWidget *widget, Size<uint> size) : NanoWidget(widget),
                                                                        fUI(ui),
                                                                        fScrollingTexture(this, size),
                                                                        fBlockSize(512),
                                                                        fSampleRate(44100),
                                                                        fMustShowGrid(true),
                                                                        fRulers(this),
                                                                        fChannelMix(WolfSpectrumPlugin::ChannelMixLRMean)
{
    setSize(size);

    fSamples = (float *)malloc(16384 * sizeof(float));
}

Spectrogram::~Spectrogram()
{
    free(fSamples);
}

double windowHanning(int i, int transformSize)
{
    return (0.5 * (1.0 - cos(2.0 * M_PI * (double)i / (double)(transformSize - 1))));
}

void Spectrogram::clear()
{
    fScrollingTexture.clear();
}

void Spectrogram::setLogFrequencyScaling(bool yesno)
{
    fLogFrequencyScaling = yesno;

    clear();
}

void Spectrogram::setHorizontalScrolling(bool yesno)
{
    fHorizontalScrolling = yesno;
    fScrollingTexture.setHorizontalScrolling(yesno);
}

void Spectrogram::setSampleRate(const double sampleRate)
{
    fSampleRate = sampleRate;
}

void Spectrogram::onResize(const ResizeEvent &ev)
{
    fScrollingTexture.setSize(ev.size);
    fRulers.setSize(ev.size.getWidth(), 32);
}

float getPowerSpectrumdB(const fftw_complex *out, const int index, const int transformSize)
{
    const float real = out[index][0] * (2.0 / transformSize);
    const float complex = out[index][1] * (2.0 / transformSize);

    const float powerSpectrum = real * real + complex * complex;
    float powerSpectrumdB = 10.0 / log(10.0) * log(powerSpectrum + 1e-9);

    // Normalize values
    powerSpectrumdB = 1.0 - powerSpectrumdB / -90.0;

    if (powerSpectrumdB > 1)
    {
        powerSpectrumdB = 1;
    }

    // Threshold
    /* if (powerSpectrumdB < 0.5)
            {
                powerSpectrumdB = 0;
            } */

    return powerSpectrumdB;
}

Color getBinPixelColor(const float powerSpectrumdB)
{
    const double euler = std::exp(1.0);
    const float scaledSpectrum = (std::exp(powerSpectrumdB) - 1) / (euler - 1);

    int dB = -90 + 90 * scaledSpectrum;
    dB = wolf::clamp(dB, -90, 0);
    dB = std::abs(dB);

    const int colorCount = 10;
    const Color colorRamp[colorCount] = {
        WolfSpectrumConfig::color_ramp_1,
        WolfSpectrumConfig::color_ramp_2,
        WolfSpectrumConfig::color_ramp_3,
        WolfSpectrumConfig::color_ramp_4,
        WolfSpectrumConfig::color_ramp_5,
        WolfSpectrumConfig::color_ramp_6,
        WolfSpectrumConfig::color_ramp_7,
        WolfSpectrumConfig::color_ramp_8,
        WolfSpectrumConfig::color_ramp_9,
        WolfSpectrumConfig::color_ramp_10};

    const int colorIndex = dB / 10;

    Color baseColor = colorRamp[colorIndex];

    if (colorIndex == 0 || colorIndex == colorCount - 1)
        return baseColor;

    Color targetColor = colorRamp[colorIndex + 1];

    baseColor.interpolate(targetColor, (dB % 10) / 10.f);

    return baseColor;
}

float getBinPos(const int bin, const int numBins, const double sampleRate)
{
    const float maxFreq = sampleRate / 2;
    const float hzPerBin = maxFreq / numBins;

    const float freq = hzPerBin * bin;
    const float scaledFreq = wolf::invLogScale(freq + 1, 20, maxFreq) - 1;

    return numBins * scaledFreq / maxFreq;
}

void Spectrogram::process(float *samples, uint32_t numSamples)
{
    if (samples == nullptr)
        return;

    const float width = getWidth();

    int transform_size = fBlockSize;
    int half = transform_size / 2;
    int step_size = transform_size / 2;

    double in[transform_size];

    fftw_complex *out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * transform_size);
    fftw_plan p = fftw_plan_dft_r2c_1d(transform_size, in, out, FFTW_ESTIMATE);

    const float scaleX = width / numSamples;
    fScrollingTexture.setScaleX(scaleX);

    for (uint32_t x = 0; x < numSamples / step_size; ++x)
    {
        for (uint32_t j = 0, i = x * step_size; i < x * step_size + transform_size; ++i, ++j)
        {
            in[j] = samples[i] * windowHanning(j, transform_size);
        }

        fftw_execute(p);

        for (int i = 0; i < half; ++i)
        {
            const float powerSpectrumdB = getPowerSpectrumdB(out, i, transform_size);

            Color pixelColor = getBinPixelColor(powerSpectrumdB);

            const int freqSize = 1;
            float freqPos = i * freqSize;

            if (fLogFrequencyScaling && i != half - 1) //must lerp to fill the gaps
            {
                const float nextPowerSpectrumdB = getPowerSpectrumdB(out, i + 1, transform_size);

                freqPos = getBinPos(i, half, fSampleRate);
                const int nextFreqPos = getBinPos(i + 1, half, fSampleRate);

                const int freqDelta = nextFreqPos - freqPos;

                for (int j = freqPos; j < nextFreqPos; ++j)
                {
                    Color lerpedColor = getBinPixelColor(wolf::lerp(powerSpectrumdB, nextPowerSpectrumdB, (j - freqPos) / (float)freqDelta));
                    fScrollingTexture.drawPixelOnCurrentLine(j, lerpedColor);
                }
            }

            fScrollingTexture.drawPixelOnCurrentLine(freqPos, pixelColor);
        }

        fScrollingTexture.scroll();
    }

    fftw_destroy_plan(p);
    fftw_free(out);
}

void Spectrogram::setBlockSize(int blockSize)
{
    fBlockSize = blockSize;
    fScrollingTexture.setBlockSize(blockSize);
}

void SpectrogramRulers::drawLogScaleGrid()
{
    const int max = fParent->fSampleRate / 2;

    for (int i = 1; i < 5; ++i)
    {
        for (int j = 1; j < 10; ++j)
        {
            const int frequency = std::pow(10, i) * j;

            if (frequency > max)
                break;

            // TODO: make that less ridiculous
            const bool mustShowCaption = frequency == 50 ||
                                         frequency == 100 ||
                                         frequency == 200 ||
                                         frequency == 500 ||
                                         frequency == 1000 ||
                                         frequency == 2000 ||
                                         frequency == 5000 ||
                                         frequency == 10000;

            if (!mustShowCaption)
                continue;

            strokeColor(Color(220, 220, 220, 255));
            strokeWidth(2.0f);
            fontSize(14.0f);
            fillColor(Color(220, 220, 220, 255));

            const String frequencyCaption = frequency >= 1000 ? String(frequency / 1000) + String("K") : String(frequency);

            const int position = wolf::invLogScale(frequency, 20, max);
            const int x = getWidth() * position / max;
            const int y = getHeight() - (getHeight() * position / max);

            beginPath();

            if (fParent->fHorizontalScrolling)
            {
                textAlign(ALIGN_MIDDLE);
                const int leftPadding = 5;
                text(leftPadding, y, frequencyCaption, NULL);
            }
            else
            {
                textAlign(ALIGN_CENTER | ALIGN_TOP);
                const int topPadding = 2;
                text(x, topPadding, frequencyCaption, NULL);
                moveTo(x, 14 + topPadding);
                lineTo(x, 24);
                stroke();
            }

            closePath();
        }
    }
}

void SpectrogramRulers::drawLinearScaleGrid()
{
    const int max = fParent->fSampleRate / 2;

    translate(0.5f, 0.5f);

    for (int i = 2000; i < max - 2000; i += 2000)
    {
        beginPath();

        strokeColor(Color(25, 25, 28, 255));
        strokeWidth(1.0f);
        fontSize(12.0f);
        fillColor(Color(200, 200, 200, 255));

        const String frequencyCaption = String(i / 1000) + String("K");
        const int x = getWidth() * i / max;
        const int y = getHeight() - (getHeight() * i / max);

        if (fParent->fHorizontalScrolling)
        {
            moveTo(0, y);
            lineTo(getWidth(), y);
        }
        else
        {
            moveTo(x, 0);
            lineTo(x, getHeight());
        }

        stroke();
        closePath();

        translate(-0.5f, -0.5f);

        beginPath();

        if (fParent->fHorizontalScrolling)
        {
            textAlign(ALIGN_MIDDLE);
            const int leftPadding = 5;
            text(leftPadding, y, frequencyCaption, NULL);
        }
        else
        {
            textAlign(ALIGN_CENTER | ALIGN_TOP);
            const int topPadding = 5;
            text(x, topPadding, frequencyCaption, NULL);
        }

        closePath();

        translate(0.5f, 0.5f);
    }

    translate(-0.5f, -0.5f);
}

void Spectrogram::setGridVisibility(bool visible)
{
    fMustShowGrid = visible;
}

void Spectrogram::setChannelMix(const int channelMix)
{
    fChannelMix = channelMix;
}

void Spectrogram::onNanoDisplay()
{
    if (WolfSpectrumPlugin *const dspPtr = (WolfSpectrumPlugin *)fUI->getPluginInstancePointer())
    {
        const MutexLocker csm(dspPtr->fMutex);

        while (dspPtr->fRingbufferL.count() >= fBlockSize)
        {
            for (int i = 0; i < fBlockSize; ++i)
            {
                const float sampleL = dspPtr->fRingbufferL.get();
                const float sampleR = dspPtr->fRingbufferR.get();

                switch (fChannelMix)
                {
                    case WolfSpectrumPlugin::ChannelMixLRMean:
                        fSamples[i] = (sampleL + sampleR) / 2.0f;
                        break;
                    case WolfSpectrumPlugin::ChannelMixL:
                        fSamples[i] = sampleL;
                        break;
                    case WolfSpectrumPlugin::ChannelMixR:
                        fSamples[i] = sampleR;
                        break;
                    default:
                        return; //¯\_(ツ)_/¯
                }
            }

            process(fSamples, fBlockSize / 2);
        }
    }
}

END_NAMESPACE_DISTRHO
