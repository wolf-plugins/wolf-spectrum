
#include "Spectrogram.hpp"
#include "DistrhoUI.hpp"

#include "Window.hpp"
#include "Mathf.hpp"

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>

START_NAMESPACE_DISTRHO

PixelDrawingSurface::PixelDrawingSurface(NanoWidget *widget, Size<uint> size, int imageFlags) : NanoWidget(widget),
                                                                                                fDirty(true),
                                                                                                fScaleX(1.0f),
                                                                                                fBufferWidth(256),
                                                                                                fBufferHeight(3012)
{
    setSize(size);

    NVGcontext *context = getContext();

    fImageData = (unsigned char *)malloc(fBufferWidth * fBufferHeight * 4);
    fFileId = nvgCreateImageRGBA(context, fBufferWidth, fBufferHeight, imageFlags, fImageData);
}

void PixelDrawingSurface::setScaleX(float scale)
{
    fScaleX = scale;
}

void PixelDrawingSurface::clear()
{
    memset(fImageData, 0, fBufferWidth * fBufferHeight * 4);
}

void PixelDrawingSurface::drawPixel(int posX, int posY, Color color)
{
    const int width = fBufferWidth;
    const int height = fBufferHeight;

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

    NVGpaint paint = nvgImagePattern(context, 0, 0, fBufferWidth, fBufferHeight, 0, fFileId, 1.0f);

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

void ScrollingTexture::clear()
{
    textureA.clear();
    textureB.clear();
}

void ScrollingTexture::onResize(const ResizeEvent &ev)
{
    textureA.setSize(ev.size);
    textureB.setSize(ev.size);

    positionTextures();
}

void ScrollingTexture::positionTextures()
{
    if (horizontalScrolling)
    {
        textureA.setAbsoluteX(getAbsoluteX() - getWidth());
        textureB.setAbsoluteX(getAbsoluteX());

        textureA.setAbsoluteY(getAbsoluteY());
        textureB.setAbsoluteY(getAbsoluteY());
    }
    else
    {
        textureA.setAbsoluteX(getAbsoluteX());
        textureB.setAbsoluteX(getAbsoluteX());

        textureA.setAbsoluteY(getAbsoluteY() - getHeight());
        textureB.setAbsoluteY(getAbsoluteY());
    }
}

void ScrollingTexture::setHorizontalScrolling(bool yesno)
{
    horizontalScrolling = yesno;

    positionTextures();
}

void ScrollingTexture::drawPixelOnCurrentLine(float pos, Color color)
{
    if (horizontalScrolling)
    {
        //flip drawing pos
        pos = getHeight() - pos;

        const float posXA = textureA.getAbsoluteX();
        const float posXB = textureB.getAbsoluteX();

        if (posXA <= getAbsoluteX())
        {
            textureA.drawPixel(std::abs(posXA), pos, color);
        }
        else
        {
            textureB.drawPixel(std::abs(posXB), pos, color);
        }
    }
    else
    {
        const float posYA = textureA.getAbsoluteY();
        const float posYB = textureB.getAbsoluteY();

        if (posYA <= getAbsoluteY())
        {
            textureA.drawPixel(pos, std::abs(posYA), color);
        }
        else
        {
            textureB.drawPixel(pos, std::abs(posYB), color);
        }
    }
}

void ScrollingTexture::setBlockSize(int blockSize)
{
    this->blockSize = blockSize;
}

void ScrollingTexture::horizontalScroll()
{
    const float posXParent = getAbsoluteX();
    const float rightParent = posXParent + getWidth();
    const float textureADest = posXParent - textureA.getWidth() + 1;
    const float textureBDest = posXParent - textureB.getWidth() + 1;
    const float posXA = textureA.getAbsoluteX() + 1;
    const float posXB = textureB.getAbsoluteX() + 1;

    if (posXA > rightParent)
    {
        textureA.setAbsoluteX(textureADest);
    }
    else
    {
        textureA.setAbsoluteX(posXA);
    }

    if (posXB > rightParent)
    {
        textureB.setAbsoluteX(textureBDest);
    }
    else
    {
        textureB.setAbsoluteX(posXB);
    }
}

void ScrollingTexture::verticalScroll()
{
    const float posYParent = getAbsoluteY();
    const float bottomParent = posYParent + getHeight();
    const float textureADest = posYParent - textureA.getHeight() + 1;
    const float textureBDest = posYParent - textureB.getHeight() + 1;
    const float posYA = textureA.getAbsoluteY() + 1;
    const float posYB = textureB.getAbsoluteY() + 1;

    if (posYA > bottomParent)
    {
        textureA.setAbsoluteY(textureADest);
    }
    else
    {
        textureA.setAbsoluteY(posYA);
    }

    if (posYB > bottomParent)
    {
        textureB.setAbsoluteY(textureBDest);
    }
    else
    {
        textureB.setAbsoluteY(posYB);
    }
}

void ScrollingTexture::scroll()
{
    ++scrollTicks;

    if (scrollTicks < blockSize)
        return;

    scrollTicks = 0;

    if (horizontalScrolling)
    {
        horizontalScroll();
    }
    else
    {
        verticalScroll();
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