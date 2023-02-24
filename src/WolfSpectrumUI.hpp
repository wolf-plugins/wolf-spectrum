// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WOLF_SPECTRUM_UI_HPP_INCLUDED
#define WOLF_SPECTRUM_UI_HPP_INCLUDED

#include "DistrhoUI.hpp"

#include "Spectrogram.hpp"
#include "ResizeHandle.hpp"

#include "WolfSpectrumPlugin.hpp"

START_NAMESPACE_DISTRHO

class Spectrogram;

class WolfSpectrumUI : public UI,
                       public ResizeHandle::Callback
{
public:
    WolfSpectrumUI();
    ~WolfSpectrumUI();

    float getParameterValue(uint32_t index) const;

protected:
    void parameterChanged(uint32_t, float value) override;
    void tryRememberSize();
    void positionWidgets(uint width, uint height);

    void resizeHandleMoved(int width, int height);

    void onNanoDisplay() override;
    void uiIdle() override;
    void uiReshape(uint width, uint height) override;
    bool onKeyboard(const KeyboardEvent &ev) override;
    bool onMouse(const MouseEvent &ev) override;
    void sampleRateChanged(const double sampleRate) override;

private:
    /**
   * Sets a parameter's value and calls parameterChanged afterwards.
   */
    void setParameterValueFeedback(uint32_t index, float value);
    void toggleFullscreen();

    float fParameters[paramCount];

    ScopedPointer<ResizeHandle> fResizeHandle;
    ScopedPointer<Spectrogram> fSpectrogram;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WolfSpectrumUI)
};

END_NAMESPACE_DISTRHO

#endif
