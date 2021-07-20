dpf_add_plugin(wolf-spectrum
	TARGETS jack lv2 vst2 dssi
        FILES_DSP
                Utils/src/Mathf.cpp
                WolfSpectrumPlugin.cpp

        FILES_UI
                Utils/src/Mathf.cpp
                Structures/src/Margin.cpp
                Structures/src/Animation.cpp
                Config/src/Config.cpp
                Libs/inih/ini.c
                Libs/inih/INIReader.cpp
                Resources/Fonts/chivo_bold.cpp
                Resources/Fonts/chivo_italic.cpp
                Widgets/src/ResizeHandle.cpp
                ScrollingTexture.cpp
                Spectrogram.cpp
                Libs/kissfft/kiss_fft.cpp
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
        "./Config"
        "./Libs/inih"
        "./Libs/kissfft"
        "./Libs/readerwriterqueue"
)

target_link_libraries(wolf-spectrum PUBLIC Threads::Threads)
