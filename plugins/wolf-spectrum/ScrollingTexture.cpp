
#include "Spectrogram.hpp"
#include "DistrhoUI.hpp"
#include "src/nanovg/nanovg.h"

#include "Window.hpp"
#include "Mathf.hpp"

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>

START_NAMESPACE_DISTRHO

PixelDrawingSurface::PixelDrawingSurface(NanoWidget *widget, Size<uint> size) : NanoWidget(widget),
                                                                                fDirty(true),
                                                                                fScaleX(1.0f)
{
    setSize(size);

    const float width = size.getWidth();
    const float height = size.getHeight();
    NVGcontext *context = getContext();

    fImageData = (unsigned char *)malloc(width * height * 4);
    fFileId = nvgCreateImageRGBA(context, width, height, 0, fImageData);
}

void PixelDrawingSurface::setScaleX(float scale)
{
    fScaleX = scale;
}

void PixelDrawingSurface::drawPixel(int posX, int posY, Color color)
{
    const int width = getWidth();
    const int height = getHeight();

    if (posX < 0 || posX >= width || posY < 0 || posY >= height)
        return;

    const int index = posY * (width * 4) + (posX * 4);

    fImageData[index + 0] = color.rgba[0] * 255;
    fImageData[index + 1] = color.rgba[1] * 255;
    fImageData[index + 2] = color.rgba[2] * 255;
    fImageData[index + 3] = color.rgba[3] * 255;

    fDirty = true;
}

void PixelDrawingSurface::onNanoDisplay()
{
    const float width = getWidth();
    const float height = getHeight();

    NVGcontext *context = getContext();

    if (fDirty)
    {
        nvgUpdateImage(context, fFileId, fImageData);
        fDirty = false;
    }

    NVGpaint paint = nvgImagePattern(context, 0, 0, width, height, 0, fFileId, 1.0f);

    beginPath();

    scale(fScaleX, 1);

    fillPaint(paint);
    rect(0, 0, width, height);
    fill();

    closePath();
}

ScrollingTexture::ScrollingTexture(NanoWidget *widget, Size<uint> size) : NanoWidget(widget),
                                                                          textureA(this, size),
                                                                          textureB(this, size),
                                                                          scrollTicks(0)
{
    setSize(size);

    textureA.setAbsoluteY(getAbsoluteY() - getHeight());
    textureB.setAbsoluteY(getAbsoluteY());

    textureA.setId(0);
    textureB.setId(1);
}

ScrollingTexture::~ScrollingTexture()
{
}

void ScrollingTexture::drawPixelOnCurrentLine(float posX, Color color)
{
    const float posYA = textureA.getAbsoluteY();
    const float posYB = textureB.getAbsoluteY();

    if(posYA <= getAbsoluteY())
    {
        textureA.drawPixel(posX, std::abs(posYA), color);
    }
    else
    {
        textureB.drawPixel(posX, std::abs(posYB), color);
    }
}

void ScrollingTexture::scroll()
{
    ++scrollTicks;

    if(scrollTicks < 256)
        return;

    scrollTicks = 0;

    const float posYParent = getAbsoluteY();
    const float bottomParent = posYParent + getHeight();
    const float textureADest = posYParent - textureA.getHeight() + 1;
    const float textureBDest = posYParent - textureB.getHeight() + 1;
    const float posYA = textureA.getAbsoluteY() + 1;
    const float posYB = textureB.getAbsoluteY() + 1;
    
    if(posYA > bottomParent)
    {
        textureA.setAbsoluteY(textureADest);
    }
    else
    {
        textureA.setAbsoluteY(posYA);
    }

    if(posYB > bottomParent)
    {
        textureB.setAbsoluteY(textureBDest);
    }
    else
    {
        textureB.setAbsoluteY(posYB);
    }
}

void ScrollingTexture::onNanoDisplay()
{
}

void ScrollingTexture::setScaleX(float scale)
{
    textureA.setScaleX(scale);
    textureB.setScaleX(scale);
}

END_NAMESPACE_DISTRHO