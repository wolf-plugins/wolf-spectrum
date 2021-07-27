
#include "Spectrogram.hpp"
#include "Config.hpp"
#include "DistrhoUI.hpp"
#include "MIDIUtils.hpp"
#include "Mathf.hpp"
#include "Window.hpp"
#include "WolfSpectrumPlugin.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

START_NAMESPACE_DISTRHO

SpectrogramRulers::SpectrogramRulers(Spectrogram *parent)
    : NanoSubWidget(parent),
      fParent(parent)
{
    loadSharedResources();
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

Spectrogram::Spectrogram(UI *ui, Widget *widget, Size<uint> size)
    : NanoSubWidget(widget),
      fUI(ui),
      fSamples(MAX_BLOCK_SIZE),
      fLogFrequencyScaling(true),
      fFFTConfig(nullptr),
      fScrollingTexture(this, size),
      fBlockSize(512),
      fHorizontalScrolling(false),
      fSampleRate(44100),
      fMustShowGrid(true),
      fChannelMix(WolfSpectrumPlugin::ChannelMixLRMean),
      fPeakFall(WolfSpectrumPlugin::PeakFallNormal),
      fThreshold(-90.f),
      fRulers(this),
      fStatusBar(this, Size<uint>(size.getWidth(), 24))
{
    setSize(size);

    fStatusBar.setAbsolutePos(0, getHeight() - StatusBar::DEFAULT_HEIGHT);
    fStatusBar.setLeftText("Wolf Spectrum v1.1.0");

    updateCoeffs();
    updateFFTConfig();
}

Spectrogram::~Spectrogram()
{
    if (fFFTConfig != nullptr)
    {
        kiss_fft_free(fFFTConfig);
    }
}

void Spectrogram::updateCoeffs()
{
    for (int i = 0; i < MAX_BLOCK_SIZE / 2; ++i)
    {
        fBins[i].calculateCoeff(fSampleRate / 64.0f, fSampleRate);
    }
}

void Spectrogram::updateFFTConfig()
{
    if (fFFTConfig != nullptr)
    {
        kiss_fft_free(fFFTConfig);
    }

    fFFTConfig = kiss_fft_alloc(fBlockSize, 0, NULL, NULL);
}

double windowHanning(int i, int transformSize)
{
    return (0.5 * (1.0 - cos(2.0 * M_PI * (double)i / (double)(transformSize - 1))));
}

void Spectrogram::clear()
{
    fScrollingTexture.clear();
}

void Spectrogram::setPeakFall(const int peakFall)
{
    fPeakFall = peakFall;
}

void Spectrogram::setLogFrequencyScaling(bool yesno)
{
    fLogFrequencyScaling = yesno;

    clear();
}

void Spectrogram::repositionRulers()
{
    fRulers.setAbsolutePos(getAbsolutePos());

    if (fHorizontalScrolling)
    {
        fRulers.setSize(32, getHeight());
    }
    else
    {
        fRulers.setSize(getWidth(), 32);
    }
}

void Spectrogram::setHorizontalScrolling(bool yesno)
{
    fHorizontalScrolling = yesno;
    fScrollingTexture.setHorizontalScrolling(yesno);

    repositionRulers();
}

void Spectrogram::setSampleRate(const double sampleRate)
{
    fSampleRate = sampleRate;

    updateCoeffs();
}

void Spectrogram::onResize(const ResizeEvent &ev)
{
    fScrollingTexture.setSize(ev.size);

    fStatusBar.setWidth(ev.size.getWidth());
    fStatusBar.setAbsolutePos(0, getHeight() - StatusBar::DEFAULT_HEIGHT);

    repositionRulers();
}

float Spectrogram::getPowerSpectrumdB(const kiss_fft_cpx *out, const int index, const int transformSize)
{
    const float real = out[index].r * (2.0 / transformSize);
    const float complex = out[index].i * (2.0 / transformSize);

    const float powerSpectrum = real * real + complex * complex;
    float powerSpectrumdB = 10.0f / std::log(10.0f) * std::log(powerSpectrum + 1e-9);

    if (powerSpectrumdB <= fThreshold)
    {
        powerSpectrumdB = -90.0f;
    }
    else
    {
        //powerSpectrumdB = wolf::lerp(-90.0f, 0.0f, 1.0f - (powerSpectrumdB + fThreshold) / -90.0f);
    }

    // Normalize values
    powerSpectrumdB = 1.0 - powerSpectrumdB / -90.0;

    if (powerSpectrumdB > 1)
    {
        powerSpectrumdB = 1;
    }

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
    const float scaledFreq = wolf::invLogScale(freq + 1, SPECTROGRAM_MIN_FREQ, maxFreq) - 1;

    return numBins * scaledFreq / maxFreq;
}

void Spectrogram::draw()
{
    const float width = getWidth();
    const float scaleX = width / fBlockSize * 2;

    fScrollingTexture.setScaleX(scaleX);

    const int half = fBlockSize / 2;

    for (int i = 0; i < half; ++i)
    {
        const float powerSpectrumdB = fBins[i].getSmoothedValue();

        Color pixelColor = getBinPixelColor(powerSpectrumdB);

        const int freqSize = 1;
        float freqPos = i * freqSize;

        if (fLogFrequencyScaling && i < half - 1) // must interpolate to fill the gaps
        {
            const float nextPowerSpectrumdB = fBins[i + 1].getSmoothedValue();

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

void Spectrogram::process()
{
    const float width = getWidth();

    int stepSize = fBlockSize / 2;
    int half = fBlockSize / 2;

    const float scaleX = width / fBlockSize * 2;
    fScrollingTexture.setScaleX(scaleX);

    // samples to throw away
    for (int i = 0; i < stepSize; ++i)
    {
        fFFTIn[i].r = fSamples.get() * windowHanning(i, fBlockSize);
        fFFTIn[i].i = 0;
    }

    // samples to keep
    for (int i = stepSize; i < stepSize * 2; ++i)
    {
        fFFTIn[i].r = fSamples.peek(i - stepSize) * windowHanning(i, fBlockSize);
        fFFTIn[i].i = 0;
    }

    kiss_fft(fFFTConfig, fFFTIn, fFFTOut);

    for (int i = 0; i < half; ++i)
    {
        fBins[i].setValue(getPowerSpectrumdB(fFFTOut, i, fBlockSize));
    }
}

void Spectrogram::setBlockSize(int blockSize)
{
    fBlockSize = blockSize;
    fScrollingTexture.setBlockSize(blockSize);
    updateFFTConfig();
}

void Spectrogram::setThreshold(const float threshold)
{
    fThreshold = threshold;
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

            // TODO: adaptive frequency grid
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

            const int position = wolf::invLogScale(frequency, SPECTROGRAM_MIN_FREQ, max);
            const int x = getWidth() * position / max;
            const int y = getHeight() - (getHeight() * position / max);

            beginPath();

            if (fParent->fHorizontalScrolling)
            {
                textAlign(ALIGN_LEFT | ALIGN_MIDDLE);
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

    for (int i = 2000; i < max - 2000; i += 2000)
    {
        beginPath();

        strokeColor(Color(220, 220, 220, 255));
        strokeWidth(2.0f);
        fontSize(14.0f);
        fillColor(Color(220, 220, 220, 255));

        const String frequencyCaption = String(i / 1000) + String("K");
        const int x = getWidth() * i / max;
        const int y = getHeight() - (getHeight() * i / max);

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

void Spectrogram::setGridVisibility(bool visible)
{
    fMustShowGrid = visible;
}

void Spectrogram::setChannelMix(const int channelMix)
{
    fChannelMix = channelMix;
}

void Spectrogram::drawFrequencyAtMouse()
{
    const float maxFreq = fSampleRate / 2.f;
    const float marginLeft = SPECTROGRAM_MIN_FREQ / maxFreq;

    const float frequency = wolf::logScale(fMousePos.getX() / getWidth(), marginLeft, 1.f) * maxFreq;

    std::ostringstream frequencyLabel;
    frequencyLabel.precision(3);
    frequencyLabel << std::fixed << frequency << " Hz";

    fStatusBar.setFrequencyText(frequencyLabel.str());

    const auto noteName = wolf::frequencyToNoteName(frequency);

    if (!noteName.empty())
    {
        fStatusBar.setNoteText("(" + noteName + ")");
    }
    else
    {
        fStatusBar.setNoteText("");
    }
}

bool Spectrogram::onMouse(const MouseEvent &ev)
{
    if (ev.button == 1)
    {
        fMouseDown = ev.press;
        fMousePos = ev.pos;

        return true;
    }

    return false;
}

bool Spectrogram::onMotion(const MotionEvent &ev)
{
    if (fMouseDown)
    {
        fMousePos = ev.pos;
        return true;
    }

    return false;
}

void Spectrogram::onNanoDisplay()
{
    draw();

    if (WolfSpectrumPlugin *const dspPtr = (WolfSpectrumPlugin *)fUI->getPluginInstancePointer())
    {
        auto ringbuffer = &dspPtr->fRingbuffer;

        while (ringbuffer->isDataAvailableForReading())
        {
            const float sample = ringbuffer->readFloat();
            fSamples.add(sample);

            if (fSamples.count() >= fBlockSize)
            {
                process();
            }
        }
    }

    draw();

    if (fMouseDown)
    {
        fStatusBar.show();
        drawFrequencyAtMouse();
    }
    else
    {
        fStatusBar.hide();
    }
}

END_NAMESPACE_DISTRHO
