#ifndef SPECTROGRAM_HPP_INCLUDED
#define SPECTROGRAM_HPP_INCLUDED

#include "DistrhoPluginInfo.h"
#include "NanoVG.hpp"
#include "WolfSpectrumPlugin.hpp"
#include "ScrollingTexture.hpp"
#include "RightClickMenu.hpp"

START_NAMESPACE_DISTRHO

class Spectrogram;

class SpectrogramRulers : public NanoWidget
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

class Spectrogram : public NanoWidget,
                    public RightClickMenu::Callback
{
  friend class SpectrogramRulers;

public:
  Spectrogram(UI *ui, NanoWidget *widget, Size<uint> size);
  ~Spectrogram();

  void process(float **samples, uint32_t numSamples);

  void setParameterValue(uint32_t i, float v);
  void setLogFrequencyScaling(bool yesno);
  void setBlockSize(int blockSize);
  void setHorizontalScrolling(bool yesno);
  void setSampleRate(const double sampleRate);
  void setGridVisibility(bool visible);

  void clear();

protected:
  enum class SpectrogramRightClickMenuItems
  {
    FrequencyScalingLogarithmic = 0,
    FrequencyScalingLinear,
    ScrollDirectionVertical,
    ScrollDirectionHorizontal,
    BlockSize64,
    BlockSize128,
    BlockSize256,
    BlockSize512,
    BlockSize1024,
    BlockSize2048,
    BlockSize4096,
    BlockSize8192,
    BlockSize16384,
    ToggleGrid
  };

  void onResize(const ResizeEvent &ev) override;
  void onNanoDisplay() override;
  bool onMouse(const MouseEvent &ev) override;
  void rightClickMenuItemSelected(RightClickMenuItem *rightClickMenuItem);

private:
  UI *fUI;
  float **fSamples;
  bool fLogFrequencyScaling;
  ScrollingTexture fScrollingTexture;
  int fBlockSize;
  bool fHorizontalScrolling;
  double fSampleRate;
  bool fMustShowGrid;
  ScopedPointer<RightClickMenu> fRightClickMenu;

  SpectrogramRulers fRulers;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spectrogram)
};

END_NAMESPACE_DISTRHO

#endif