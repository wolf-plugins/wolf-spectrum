#ifndef TEXTURE_ROLL_HPP_INCLUDED
#define TEXTURE_ROLL_HPP_INCLUDED

#include "NanoVG.hpp"

START_NAMESPACE_DISTRHO

class PixelDrawingSurface : public NanoWidget
{
  public:
    PixelDrawingSurface(NanoWidget *widget, Size<uint> size);

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

  void drawPixelOnCurrentLine(float posX, Color color);
  void scroll();
  void setScaleX(float scale);

  protected:
    void onNanoDisplay() override;

  private:    
    PixelDrawingSurface textureA;
    PixelDrawingSurface textureB;

    int scrollTicks;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScrollingTexture)
};

END_NAMESPACE_DISTRHO

#endif