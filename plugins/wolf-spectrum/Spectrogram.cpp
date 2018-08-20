
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
                                                                        fUI(ui)
{
    setSize(size);

    fSamples = (float **)malloc(sizeof(float *) * 2);

    fSamples[0] = (float *)malloc(1024 * sizeof(float));
    fSamples[1] = (float *)malloc(1024 * sizeof(float));
}

Spectrogram::~Spectrogram()
{
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

    int transform_size = 512;
    int half = transform_size / 2;
    int step_size = transform_size / 2;

    double in[transform_size];
    double processed[half];

    fftw_complex *out;
    fftw_plan p;

    out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * transform_size);

    const float zoomX = width / numSamples;
    scale(zoomX, 1);

    for (uint32_t x = 0; x < numSamples / step_size; ++x)
    {
        // Fill the transformation array with a sample frame and apply the window function.
        // Normalization is performed later
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
            processed[i] = out[i][0] * out[i][0] + out[i][1] * out[i][1]; // power spectrum
            processed[i] = 10.0 / log(10.0) * log(processed[i] + 1e-9);   // dB

            // The resulting spectral values in 'processed' are in dB and related to a maximum
            // value of about 90dB.

            // Transform all dB values to a range between 0 and 1:
            processed[i] = 1.0 - processed[i] / -90.0; // Reduce the divisor if you prefer darker peaks

            if (processed[i] > 1)
            {
                processed[i] = 1;
            }

            beginPath();

            fillColor(Color::fromHSL((175 + (int)(120 * processed[i]) % 255) / 255.f, 1, 0.58, processed[i]));

            const int freqSize = 1;
            float freqPosX = i * freqSize;

            if (fLogFrequencyScaling)
            {
                freqPosX = wolf::invLogScale(freqPosX + 1, 1, half) - 1;
            }

            rect(freqPosX, 100, freqSize, freqSize);
            fill();

            closePath();
        }

        fftw_destroy_plan(p);
    }

    fftw_free(out);
}

void Spectrogram::setSamples(float **samples)
{
}

void Spectrogram::onNanoDisplay()
{
    if (WolfSpectrumPlugin *const dspPtr = (WolfSpectrumPlugin *)fUI->getPluginInstancePointer())
    {
        const MutexLocker csm(dspPtr->fMutex);

        while (dspPtr->fRingbuffer.count() >= 256)
        {
            for (int i = 0; i < 512; ++i)
            {
                fSamples[0][i] = dspPtr->fRingbuffer.get();
            }

            process(fSamples, 256);
        }
    }
}

void Spectrogram::idleCallback()
{
}

END_NAMESPACE_DISTRHO