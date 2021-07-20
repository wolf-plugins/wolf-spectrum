dpf_add_plugin(wolf-spectrum
	TARGETS jack lv2 vst2 dssi
        FILES_DSP
                Utils/src/Mathf.cpp
                WolfSpectrumPlugin.cpp

        FILES_UI
                ../libs/inih/ini.c
                ../libs/inih/INIReader.cpp
                ../libs/kissfft/kiss_fft.cpp
                Utils/src/Mathf.cpp
                Structures/src/Margin.cpp
                Structures/src/Animation.cpp
                Resources/Fonts/chivo_bold.cpp
                Resources/Fonts/chivo_italic.cpp
                Widgets/src/ResizeHandle.cpp
                ScrollingTexture.cpp
                Spectrogram.cpp
                Structures/src/ParamSmooth.cpp
                Structures/src/PeakFallSmooth.cpp
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
        "../libs/inih"
        "../libs/kissfft"
        "../libs/readerwriterqueue"
)

target_link_libraries(wolf-spectrum PUBLIC Threads::Threads)
