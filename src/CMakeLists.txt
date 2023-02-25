# SPDX-FileCopyrightText: © Patrick Desaulniers
# SPDX-License-Identifier: GPL-3.0-or-later

dpf_add_plugin(wolf-spectrum
        TARGETS jack lv2 vst2 vst3 clap
        MONOLITHIC
        FILES_DSP
                Utils/src/Mathf.cpp
                WolfSpectrumPlugin.cpp

        FILES_UI
                ../libs/kissfft/kiss_fft.c
                Utils/src/Mathf.cpp
                Structures/src/PeakFallSmooth.cpp
                Structures/src/Margin.cpp
                Widgets/src/ResizeHandle.cpp
                Widgets/src/ScrollingTexture.cpp
                Widgets/src/StatusBar.cpp
                Widgets/src/Spectrogram.cpp
                WolfSpectrumUI.cpp
)

target_include_directories(wolf-spectrum PUBLIC
	"."
	"../dpf/distrho"
	"../dpf/dgl"
	"./Structures"
	"./Widgets"
	"./Utils"
	"./Resources"
        "../libs/kissfft"
        "../libs/readerwriterqueue"
)

target_link_libraries(wolf-spectrum PUBLIC Threads::Threads)
