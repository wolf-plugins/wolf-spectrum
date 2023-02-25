// SPDX-FileCopyrightText: © Patrick Desaulniers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef STATUSBAR_HPP
#define STATUSBAR_HPP

#include "DistrhoPluginInfo.h"
#include "NanoVG.hpp"
#include <string>

START_NAMESPACE_DISTRHO

class StatusBar : public NanoSubWidget
{

public:
    StatusBar(Widget *parent, Size<uint> size);

    void setLeftText(const std::string &text);
    void setFrequencyText(const std::string &text);
    void setNoteText(const std::string &text);

    static constexpr uint DEFAULT_HEIGHT = 18;

protected:
    void onNanoDisplay() override;

private:
    std::string fLeftText;
    std::string fFrequencyText;
    std::string fNoteText;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatusBar)
};

END_NAMESPACE_DISTRHO

#endif
