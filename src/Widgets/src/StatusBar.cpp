// SPDX-FileCopyrightText: © Patrick Desaulniers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "StatusBar.hpp"

START_NAMESPACE_DISTRHO

StatusBar::StatusBar(Widget *parent, Size<uint> size)
    : NanoSubWidget(parent)
{
    setSize(size);

    loadSharedResources();
}

void StatusBar::setLeftText(const std::string &text)
{
    fLeftText = text;

    repaint();
}

void StatusBar::setFrequencyText(const std::string &text)
{
    fFrequencyText = text;

    repaint();
}

void StatusBar::setNoteText(const std::string &text)
{
    fNoteText = text;

    repaint();
}

void StatusBar::onNanoDisplay()
{
    // Draw background

    const auto width = static_cast<float>(getWidth());
    const auto height = static_cast<float>(getHeight());

    beginPath();

    fillColor(0, 0, 0, 255);
    rect(0, 0, width, height);
    fill();

    closePath();

    // Draw text

    static const uint marginLeft = 14;
    static const uint marginRight = 34;

    fontSize(14.f);

    fillColor(160, 160, 160, 255);

    if (!fLeftText.empty())
    {
        textAlign(Align::ALIGN_LEFT | Align::ALIGN_BASELINE);
        text(marginLeft, height / 2.f + 1, fLeftText.c_str(), nullptr);
    }

    fillColor(220, 220, 220, 255);

    if (!fNoteText.empty())
    {
        textAlign(Align::ALIGN_RIGHT | Align::ALIGN_BASELINE);
        text(width - marginRight, height / 2.f + 1, fNoteText.c_str(), nullptr);
    }

    if (!fFrequencyText.empty())
    {
        textAlign(Align::ALIGN_RIGHT | Align::ALIGN_BASELINE);
        text(width - marginRight - 64, height / 2.f + 1, fFrequencyText.c_str(), nullptr);
    }
}

END_NAMESPACE_DISTRHO
