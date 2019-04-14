
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
                                                                                                fScaleY(1.0f),
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

void PixelDrawingSurface::setScaleY(float scale)
{
    fScaleY = scale;
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

void PixelDrawingSurface::drawPixel(int posX, int posY, Color color, bool verticalInterpolation)
{
    const int width = INTERNAL_BUFFER_WIDTH;
    const int height = INTERNAL_BUFFER_HEIGHT;

    float realPixelSize;

    if (verticalInterpolation)
    {
        realPixelSize = (float)INTERNAL_BUFFER_HEIGHT / (float)fBufferHeight;
        posY *= realPixelSize;
    }
    else
    {
        realPixelSize = (float)INTERNAL_BUFFER_WIDTH / (float)fBufferWidth;
        posX *= realPixelSize;
    }

    DISTRHO_SAFE_ASSERT(!(posX < 0 || posX >= width || posY < 0 || posY >= height))

    const int hole = 1.0f * realPixelSize;
    const int prevPosX = posX - hole;

    if (hole > 1.0f && prevPosX >= 0) //we lerp with the previous "pixel" to fill the gaps
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

    scale(1, fScaleY * ((float)fBufferHeight / (float)INTERNAL_BUFFER_HEIGHT));

    fillPaint(paint);
    rect(0, 0, width, height);

    /*
    if (getId() == 0)
    {
        fillColor(Color(255,0,0));
    }
    else
    {
        fillColor(Color(0,255,0));
    }
    */
    fill();
    closePath();
}

void PixelDrawingSurface::clearLineHorizontal(int posY)
{
    memset(fImageData + posY * INTERNAL_BUFFER_WIDTH * 4, 0, INTERNAL_BUFFER_WIDTH * 4);
    fDirty = true;
}

void PixelDrawingSurface::clearLineVertical(int posX)
{
    for (int i = 0; i < INTERNAL_BUFFER_HEIGHT; ++i)
    {
        memset(fImageData + i * INTERNAL_BUFFER_WIDTH * 4 + posX * 4, 0, 4);
    }

    fDirty = true;
}

ScrollingTexture::ScrollingTexture(NanoWidget *widget, Size<uint> size) : NanoWidget(widget),
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
    clear();
    positionTextures();
}

void ScrollingTexture::drawPixelOnCurrentLine(int pos, Color color)
{
    if (horizontalScrolling)
    {
        //flip drawing pos
        pos = getHeight() - pos;

        const float posXA = getWidth() - textureA.getAbsoluteX();
        const float posXB = getWidth() - textureB.getAbsoluteX();

        if (posXA <= getAbsoluteX() + getWidth())
        {
            textureA.drawPixel(std::abs(posXA), pos, color, true);
        }
        else
        {
            textureB.drawPixel(std::abs(posXB), pos, color, true);
        }
    }
    else
    {
        const float posYA = getHeight() - textureA.getAbsoluteY();
        const float posYB = getHeight() - textureB.getAbsoluteY();

        if (posYA <= getAbsoluteY() + getHeight())
        {
            textureA.drawPixel(pos, std::abs(posYA), color, false);
        }
        else
        {
            textureB.drawPixel(pos, std::abs(posYB), color, false);
        }
    }
}

void ScrollingTexture::clearCurrentLine()
{
    if (horizontalScrolling)
    {
        const float posXA = getWidth() - textureA.getAbsoluteX();
        const float posXB = getWidth() - textureB.getAbsoluteX();

        if (posXA <= getAbsoluteX() + getWidth())
        {
            textureA.clearLineVertical(std::abs(posXA));
        }
        else
        {
            textureB.clearLineVertical(std::abs(posXB));
        }
    }
    else
    {
        const float posYA = getHeight() - textureA.getAbsoluteY();
        const float posYB = getHeight() - textureB.getAbsoluteY();

        if (posYA <= getAbsoluteY() + getHeight())
        {
            textureA.clearLineHorizontal(std::abs(posYA));
        }
        else
        {
            textureB.clearLineHorizontal(std::abs(posYB));
        }
    }
}

void ScrollingTexture::setBlockSize(int blockSize)
{
    this->blockSize = blockSize;

    textureA.setBufferSize(3024, blockSize);
    textureB.setBufferSize(3024, blockSize);
}

void ScrollingTexture::horizontalScroll()
{
    const float posXParent = getAbsoluteX();
    const float rightParent = posXParent + getWidth();
    const float textureADest = rightParent;
    const float textureBDest = rightParent;
    const float posXA = textureA.getAbsoluteX() - 1;
    const float posXB = textureB.getAbsoluteX() - 1;

    if (posXA + (int)textureA.getWidth() <= posXParent)
    {
        textureA.setAbsoluteX(textureADest);
    }
    else
    {
        textureA.setAbsoluteX(posXA);
    }

    if (posXB + (int)textureB.getWidth() < posXParent)
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

void ScrollingTexture::setScaleY(float scale)
{
    textureA.setScaleY(scale);
    textureB.setScaleY(scale);
}

END_NAMESPACE_DISTRHO