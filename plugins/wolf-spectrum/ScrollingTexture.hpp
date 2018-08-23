#ifndef TEXTURE_ROLL_HPP_INCLUDED
#define TEXTURE_ROLL_HPP_INCLUDED

#include "NanoVG.hpp"
#include "src/nanovg/nanovg.h"

START_NAMESPACE_DISTRHO

class PixelDrawingSurface : public NanoWidget
{
  public:
    PixelDrawingSurface(NanoWidget *widget, Size<uint> size, int imageFlags = NVG_IMAGE_NEAREST);

    void drawPixel(int posX, int posY, Color pixelColor);
    void setScaleX(float scale);
    
  protected:
    void onNanoDisplay() override;

  private:
    int fFileId;
    unsigned char *fImageData;
    bool fDirty;
    float fScaleX;
};

class ScrollingTexture : public NanoWidget
{
  public:
    ScrollingTexture(NanoWidget *widget, Size<uint> size);
    ~ScrollingTexture();

  void drawPixelOnCurrentLine(float pos, Color color);
  void scroll();
  void setScaleX(float scale);
  void setBlockSize(int blockSize);
  void setHorizontalScrolling(bool yesno);

  protected:
    void onNanoDisplay() override;

  private:  
    void verticalScroll();
    void horizontalScroll();

    PixelDrawingSurface textureA;
    PixelDrawingSurface textureB;

    int scrollTicks;
    int blockSize;

    bool horizontalScrolling;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScrollingTexture)
};

END_NAMESPACE_DISTRHO

#endif