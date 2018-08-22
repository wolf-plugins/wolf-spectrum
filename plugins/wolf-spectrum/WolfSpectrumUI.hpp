#ifndef WOLF_SPECTRUM_UI_HPP_INCLUDED
#define WOLF_SPECTRUM_UI_HPP_INCLUDED

#include "DistrhoUI.hpp"
#include "ResizeHandle.hpp"
#include "NanoLabel.hpp"

#include "WolfSpectrumPlugin.hpp"

#include "Spectrogram.hpp"

START_NAMESPACE_DISTRHO

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

private:
  ScopedPointer<ResizeHandle> fResizeHandle;
  ScopedPointer<Spectrogram> fSpectrogram;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WolfSpectrumUI)
};

END_NAMESPACE_DISTRHO

#endif