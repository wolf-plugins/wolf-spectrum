#ifndef WOLF_SPECTRUM_UI_HPP_INCLUDED
#define WOLF_SPECTRUM_UI_HPP_INCLUDED

#include "DistrhoUI.hpp"
#include "RemoveDCSwitch.hpp"
#include "OversampleWheel.hpp"
#include "VolumeKnob.hpp"
#include "ResizeHandle.hpp"
#include "LabelBox.hpp"
#include "BipolarModeSwitch.hpp"
#include "GlowingLabelsBox.hpp"
#include "NanoLabel.hpp"
#include "WidgetBar.hpp"
#include "ArrowButton.hpp"
#include "LabelBoxList.hpp"

START_NAMESPACE_DISTRHO

class WolfSpectrumUI : public UI,
                     public NanoSwitch::Callback,
                     public NanoButton::Callback,
                     public NanoWheel::Callback,
                     public NanoKnob::Callback,
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

  void nanoSwitchClicked(NanoSwitch *nanoSwitch);
  void nanoButtonClicked(NanoButton *nanoButton);
  void nanoWheelValueChanged(NanoWheel *nanoWheel, const int value);
  void nanoKnobValueChanged(NanoKnob *nanoKnob, const float value);

  void resizeHandleMoved(int width, int height);

  void onNanoDisplay() override;
  void uiIdle() override;
  void uiReshape(uint width, uint height) override;
  bool onKeyboard(const KeyboardEvent &ev) override;
  bool onMouse(const MouseEvent &ev) override;

private:
  ScopedPointer<ResizeHandle> fResizeHandle;

  float fSamplesLeft[16384];
  float fSamplesRight[16384];
  
  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WolfSpectrumUI)
};

END_NAMESPACE_DISTRHO

#endif