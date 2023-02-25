// SPDX-FileCopyrightText: © Patrick Desaulniers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "DistrhoUI.hpp"

#include "Config.hpp"
#include "Window.hpp"
#include "WolfSpectrumParameters.hpp"
#include "WolfSpectrumUI.hpp"

#include <string>

START_NAMESPACE_DISTRHO

WolfSpectrumUI::WolfSpectrumUI()
    : UI(1200, 200)
{
    const uint minWidth = 1024;
    const uint minHeight = 200;

    setGeometryConstraints(minWidth, minHeight, false, false);

    loadSharedResources();

    const float width = getWidth();
    const float height = getHeight();

    fSpectrogram = new Spectrogram(this, this, Size<uint>(width, height));
    fSpectrogram->setSampleRate(getSampleRate());

    fResizeHandle = new ResizeHandle(this, Size<uint>(18, 18));
    fResizeHandle->setCallback(this);
    fResizeHandle->setMinSize(minWidth, minHeight);

    positionWidgets(width, height);
}

WolfSpectrumUI::~WolfSpectrumUI()
{
}

void WolfSpectrumUI::setParameterValueFeedback(uint32_t index, float value)
{
    setParameterValue(index, value);
    parameterChanged(index, value);
}

void WolfSpectrumUI::positionWidgets(uint width, uint height)
{
    fResizeHandle->setAbsolutePos(width - fResizeHandle->getWidth(), height - fResizeHandle->getHeight());
    fSpectrogram->setSize(width, height);
}

void WolfSpectrumUI::parameterChanged(uint32_t index, float value)
{
    value = std::round(value);

    switch (index)
    {
    case paramFrequencyScaling:
        fSpectrogram->setLogFrequencyScaling(value == WolfSpectrumPlugin::FrequencyScaling::FrequencyScalingLogarithmic);
        break;
    case paramBlockSize:
        fSpectrogram->setBlockSize(std::pow(2, 5 + (int)value)); //careful, this assumes block size 64 is the minimum
        break;
    /* case paramScrollDirection:
        fSpectrogram->setHorizontalScrolling(value == WolfSpectrumPlugin::ScrollDirection::ScrollDirectionHorizontal);
        break; */
    case paramShowCaptions:
        fSpectrogram->setGridVisibility(value);
        break;
    case paramChannelMix:
        fSpectrogram->setChannelMix(value);
        break;
    /* case paramPeakFall:
        fSpectrogram->setPeakFall(value);
        break; */
    /* case paramThreshold:
        fSpectrogram->setThreshold(value);
        break; */
    case paramShowUIControls:
        fResizeHandle->setVisible(value);
        break;
    }

    fParameters[index] = value;
}

bool WolfSpectrumUI::onMouse(const MouseEvent &ev)
{
    return UI::onMouse(ev);
}

void WolfSpectrumUI::onNanoDisplay()
{
}

void WolfSpectrumUI::uiIdle()
{
    fSpectrogram->update();
    repaint();
}

void WolfSpectrumUI::uiReshape(uint width, uint height)
{
    positionWidgets(width, height);
}

void WolfSpectrumUI::toggleFullscreen()
{
    return;
}

bool WolfSpectrumUI::onKeyboard(const KeyboardEvent &ev)
{
    if (ev.press)
    {
        if (ev.key == 96) //F12
        {
            fSpectrogram->clear();

            return true;
        }
    }

    return false;
}

void WolfSpectrumUI::resizeHandleMoved(int width, int height)
{
    setSize(width, height);
}

void WolfSpectrumUI::sampleRateChanged(const double sampleRate)
{
    fSpectrogram->setSampleRate(sampleRate);
}

UI *createUI()
{
    return new WolfSpectrumUI();
}

END_NAMESPACE_DISTRHO
