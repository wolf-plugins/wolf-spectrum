
#include "Spectrogram.hpp"
#include "DistrhoUI.hpp"
#include "Window.hpp"
#include "Mathf.hpp"

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fftw3.h>

START_NAMESPACE_DISTRHO

Spectrogram::Spectrogram(UI *ui, NanoWidget *widget, Size<uint> size) : NanoWidget(widget),
                                                                        fUI(ui),
                                                                        fScrollingTexture(this, size),
                                                                        fBlockSize(512)
{
    setSize(size);

    fSamples = (float **)malloc(sizeof(float *) * 2);

    fSamples[0] = (float *)malloc(16384 * sizeof(float));
    fSamples[1] = (float *)malloc(16384 * sizeof(float));
}

Spectrogram::~Spectrogram()
{
    free(fSamples[1]);
    free(fSamples[0]);
    free(fSamples);
}

double windowHanning(int i, int transformSize)
{
    return (0.5 * (1.0 - cos(2.0 * M_PI * (double)i / (double)(transformSize - 1))));
}

void Spectrogram::setLogFrequencyScaling(bool yesno)
{
    fLogFrequencyScaling = yesno;
}

void Spectrogram::process(float **samples, uint32_t numSamples)
{
    if (samples == nullptr)
        return;

    const float width = getWidth();

    int transform_size = fBlockSize;
    int half = transform_size / 2;
    int step_size = transform_size / 2;

    double in[transform_size];

    fftw_complex *out;
    fftw_plan p;

    out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * transform_size);

    const float scaleX = width / numSamples;
    fScrollingTexture.setScaleX(scaleX);

    for (uint32_t x = 0; x < numSamples / step_size; ++x)
    {
        for (uint32_t j = 0, i = x * step_size; i < x * step_size + transform_size; ++i, ++j)
        {
            in[j] = samples[0][i] * windowHanning(j, transform_size);
        }

        p = fftw_plan_dft_r2c_1d(transform_size, in, out, FFTW_ESTIMATE);

        fftw_execute(p);

        for (int i = 0; i < half; ++i)
        {
            out[i][0] *= (2.0 / transform_size);                          // real values
            out[i][1] *= (2.0 / transform_size);                          // complex values

            const float powerSpectrum = out[i][0] * out[i][0] + out[i][1] * out[i][1];
            float powerSpectrumdB = 10.0 / log(10.0) * log(powerSpectrum + 1e-9);

            // Normalize values
            powerSpectrumdB = 1.0 - powerSpectrumdB / -90.0;

            if (powerSpectrumdB > 1)
            {
                powerSpectrumdB = 1;
            }

            Color pixelColor = Color::fromHSL((175 + (int)(120 * powerSpectrumdB) % 255) / 255.f, 1, 0.58, powerSpectrumdB);

            const int freqSize = 1;
            float freqPosX = i * freqSize;

            if (fLogFrequencyScaling)
            {
                freqPosX = wolf::invLogScale(freqPosX + 1, 1, half) - 1;
            }

            fScrollingTexture.drawPixelOnCurrentLine(freqPosX, pixelColor);
            fScrollingTexture.scroll();
        }

        fftw_destroy_plan(p);
    }

    fftw_free(out);
}

void Spectrogram::setBlockSize(int blockSize)
{
    fBlockSize = blockSize;
    fScrollingTexture.setBlockSize(blockSize);
}

void Spectrogram::onNanoDisplay()
{
    if (WolfSpectrumPlugin *const dspPtr = (WolfSpectrumPlugin *)fUI->getPluginInstancePointer())
    {
        const MutexLocker csm(dspPtr->fMutex);

        while (dspPtr->fRingbuffer.count() >= fBlockSize)
        {
            for (int i = 0; i < fBlockSize; ++i)
            {
                fSamples[0][i] = dspPtr->fRingbuffer.get();
            }

            process(fSamples, fBlockSize / 2);
        }
    }
}

END_NAMESPACE_DISTRHO