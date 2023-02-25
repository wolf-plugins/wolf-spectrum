// SPDX-FileCopyrightText: © Patrick Desaulniers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Margin.hpp"

START_NAMESPACE_DISTRHO

Margin::Margin(float left, float right, float top, float bottom)
    : left(left),
      right(right),
      top(top),
      bottom(bottom)
{
}

END_NAMESPACE_DISTRHO
