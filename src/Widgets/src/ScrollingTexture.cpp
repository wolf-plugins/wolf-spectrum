// SPDX-FileCopyrightText: © 2023 Patrick Desaulniers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Mathf.hpp"

#include <cmath>
#include <cstdlib>

#include "ScrollingTexture.hpp"
#include "src/nanovg/nanovg.h"

START_NAMESPACE_DISTRHO

PixelDrawingSurface::PixelDrawingSurface(Widget *widget, Size<uint> size, int imageFlags)
    : NanoSubWidget(widget),
      fDirty(true),
      fScaleX(1.0f),
      fBufferWidth(INTERNAL_BUFFER_WIDTH),
      fBufferHeight(INTERNAL_BUFFER_HEIGHT),
      fImageFlags(imageFlags)
{
    setSize(size);

    NVGcontext *context = getContext();

    fImageData = (unsigned char *)calloc(4, INTERNAL_BUFFER_WIDTH * INTERNAL_BUFFER_HEIGHT * 4);
    fFileId = nvgCreateImageRGBA(context, INTERNAL_BUFFER_WIDTH, INTERNAL_BUFFER_HEIGHT, fImageFlags, fImageData);
}

PixelDrawingSurface::~PixelDrawingSurface()
{
    nvgDeleteImage(getContext(), fFileId);
    free(fImageData);
}

void PixelDrawingSurface::setScaleX(float scale)
{
    fScaleX = scale;
}

void PixelDrawingSurface::clear()
{
    memset(fImageData, 0, INTERNAL_BUFFER_WIDTH * INTERNAL_BUFFER_HEIGHT * 4);
    fDirty = true;
}

void PixelDrawingSurface::setBufferSize(int width, int height)
{
    fBufferWidth = width;
    fBufferHeight = height;
}

void PixelDrawingSurface::drawPixel(int posX, int posY, Color color)
{
    const int width = INTERNAL_BUFFER_WIDTH;
    const int height = INTERNAL_BUFFER_HEIGHT;

    const float realPixelSize = (float)INTERNAL_BUFFER_WIDTH / (float)fBufferWidth;
    posX *= realPixelSize;

    DISTRHO_SAFE_ASSERT(!(posX < 0 || posX >= width || posY < 0 || posY >= height))

    const int hole = 1.0f * realPixelSize;
    const int prevPosX = posX - hole;

    if (hole > 1.0f && prevPosX >= 0) // we lerp with the previous "pixel" to fill the gaps
    {
        const int prevIndex = posY * (width * 4) + (prevPosX * 4);

        const float prevR = fImageData[prevIndex + 0];
        const float prevG = fImageData[prevIndex + 1];
        const float prevB = fImageData[prevIndex + 2];
        const float prevA = fImageData[prevIndex + 3];

        for (int x = posX - 1; x > prevPosX; --x)
        {
            const int index = posY * (width * 4) + (x * 4);
            const float percentComplete = (posX - x) / (float)hole;

            fImageData[index + 0] = std::max(wolf::lerp(color.rgba[0] * 255, prevR, percentComplete), (float)fImageData[index + 0]);
            fImageData[index + 1] = std::max(wolf::lerp(color.rgba[1] * 255, prevG, percentComplete), (float)fImageData[index + 1]);
            fImageData[index + 2] = std::max(wolf::lerp(color.rgba[2] * 255, prevB, percentComplete), (float)fImageData[index + 2]);
            fImageData[index + 3] = std::max(wolf::lerp(color.rgba[3] * 255, prevA, percentComplete), (float)fImageData[index + 3]);
        }
    }

    const int index = posY * (width * 4) + (posX * 4);

    fImageData[index + 0] = std::max(color.rgba[0] * 255, (float)fImageData[index + 0]);
    fImageData[index + 1] = std::max(color.rgba[1] * 255, (float)fImageData[index + 1]);
    fImageData[index + 2] = std::max(color.rgba[2] * 255, (float)fImageData[index + 2]);
    fImageData[index + 3] = std::max(color.rgba[3] * 255, (float)fImageData[index + 3]);

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

    NVGpaint paint = nvgImagePattern(context, 0, 0, INTERNAL_BUFFER_WIDTH, INTERNAL_BUFFER_HEIGHT, 0, fFileId, 1.0f);

    beginPath();

    scale(fScaleX * ((float)fBufferWidth / (float)INTERNAL_BUFFER_WIDTH), 1);

    fillPaint(paint);
    rect(0, 0, width, height);
    fill();
    closePath();
}

void PixelDrawingSurface::clearLine(int posY)
{
    memset(fImageData + posY * INTERNAL_BUFFER_WIDTH * 4, 1, INTERNAL_BUFFER_WIDTH * 4);
    fDirty = true;
}

ScrollingTexture::ScrollingTexture(Widget *widget, Size<uint> size)
    : NanoSubWidget(widget),
      textureA(this, size),
      textureB(this, size),
      scrollTicks(0),
      horizontalScrolling(false)
{
    setSize(size);

    textureA.setAbsoluteY(getAbsoluteY() + getHeight());
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
    textureA.setSize(ev.size.getWidth(), ev.size.getHeight() + 1);
    textureB.setSize(ev.size.getWidth(), ev.size.getHeight());

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

        textureA.setAbsoluteY(getAbsoluteY() + getHeight());
        textureB.setAbsoluteY(getAbsoluteY());
    }
}

void ScrollingTexture::setHorizontalScrolling(bool yesno)
{
    horizontalScrolling = yesno;

    positionTextures();
}

void ScrollingTexture::drawPixelOnCurrentLine(int pos, Color color)
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
        const float posYA = getHeight() - textureA.getAbsoluteY();
        const float posYB = getHeight() - textureB.getAbsoluteY();

        if (posYA <= getAbsoluteY() + getHeight())
        {
            textureA.drawPixel(pos, std::abs(posYA), color);
        }
        else
        {
            textureB.drawPixel(pos, std::abs(posYB), color);
        }
    }
}

void ScrollingTexture::clearCurrentLine()
{
    if (horizontalScrolling)
    {
        //TODO
    }
    else
    {
        const float posYA = getHeight() - textureA.getAbsoluteY();
        const float posYB = getHeight() - textureB.getAbsoluteY();

        if (posYA <= getAbsoluteY() + getHeight())
        {
            textureA.clearLine(std::abs(posYA));
        }
        else
        {
            textureB.clearLine(std::abs(posYB));
        }
    }
}

void ScrollingTexture::setBlockSize(int blockSize)
{
    this->blockSize = blockSize;

    textureA.setBufferSize(blockSize, 3024);
    textureB.setBufferSize(blockSize, 3024);
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
    const int posYParent = getAbsoluteY();
    const int bottomParent = posYParent + getHeight();
    const int textureADest = bottomParent;
    const int textureBDest = bottomParent;
    const int posYA = textureA.getAbsoluteY() - 1;
    const int posYB = textureB.getAbsoluteY() - 1;

    if (posYA + (int)textureA.getHeight() <= posYParent)
    {
        textureA.setAbsoluteY(textureADest);
    }
    else
    {
        textureA.setAbsoluteY(posYA);
    }

    if (posYB + (int)textureB.getHeight() < posYParent)
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
    if (horizontalScrolling)
    {
        horizontalScroll();
    }
    else
    {
        verticalScroll();
    }

    clearCurrentLine();
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
