// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "src/DistrhoDefines.h"

START_NAMESPACE_DISTRHO

struct Margin
{
    Margin(float left, float right, float top, float bottom);

    float left;
    float right;
    float top;
    float bottom;
};

END_NAMESPACE_DISTRHO
