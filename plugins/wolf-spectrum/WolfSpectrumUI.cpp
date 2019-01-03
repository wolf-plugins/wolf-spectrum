#include "DistrhoUI.hpp"

#include "WolfSpectrumUI.hpp"
#include "Window.hpp"
#include "Config.hpp"
#include "Layout.hpp"
#include "Fonts/chivo_bold.hpp"
#include "WolfSpectrumParameters.hpp"

#include <string>

#if defined(DISTRHO_OS_WINDOWS)
#include "windows.h"
#endif

START_NAMESPACE_DISTRHO

WolfSpectrumUI::WolfSpectrumUI() : UI(1200, 200)
{
    const uint minWidth = 1024;
    const uint minHeight = 200;

    const uint knobsLabelBoxWidth = 66;
    const uint knobsLabelBoxHeight = 21;

    loadSharedResources();

    using namespace WOLF_FONTS;
    NanoVG::FontId chivoBoldId = createFontFromMemory("chivo_bold", (const uchar *)chivo_bold, chivo_bold_size, 0);
    NanoVG::FontId dejaVuSansId = findFont(NANOVG_DEJAVU_SANS_TTF);

    WolfSpectrumConfig::load();

    tryRememberSize();
    getParentWindow().saveSizeAtExit(true);

    const float width = getWidth();
    const float height = getHeight();

    fSpectrogram = new Spectrogram(this, this, Size<uint>(width, height));
    fSpectrogram->setSampleRate(getSampleRate());

    fResizeHandle = new ResizeHandle(this, Size<uint>(18, 18));
    fResizeHandle->setCallback(this);
    fResizeHandle->setMinSize(minWidth, minHeight);

    fRightClickMenu = new RightClickMenu(this);

    fRightClickMenu->addSection("Frequency scaling");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::FrequencyScalingLogarithmic, "Logarithmic");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::FrequencyScalingLinear, "Linear");

    fRightClickMenu->addSection("Scrolling direction");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::ScrollDirectionVertical, "Vertical");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::ScrollDirectionHorizontal, "Horizontal");

    fRightClickMenu->addSection("Block size");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::BlockSize64, "64 samples");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::BlockSize128, "128 samples");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::BlockSize256, "256 samples");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::BlockSize512, "512 samples");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::BlockSize1024, "1024 samples");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::BlockSize2048, "2048 samples");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::BlockSize4096, "4096 samples");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::BlockSize8192, "8192 samples");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::BlockSize16384, "16384 samples");

    fRightClickMenu->addSection("Channel mix");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::ChannelMixLRMean, "Left/Right (mean)");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::ChannelMixL, "Left");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::ChannelMixR, "Right");

    fRightClickMenu->addSection("Captions");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::ToggleCaptions, "Toggle on/off");

    fRightClickMenu->addSection("UI controls");
    fRightClickMenu->addItem((int)SpectrogramRightClickMenuItems::ToggleUIControls, "Toggle on/off");

    fRightClickMenu->setCallback(this);

    positionWidgets(width, height);
}

WolfSpectrumUI::~WolfSpectrumUI()
{
}

void WolfSpectrumUI::tryRememberSize()
{
    int width, height;
    FILE *file;
    std::string tmpFileName = PLUGIN_NAME ".tmp";

#if defined(DISTRHO_OS_WINDOWS)
    CHAR tempPath[MAX_PATH + 1];

    GetTempPath(MAX_PATH + 1, tempPath);
    std::string path = std::string(tempPath) + tmpFileName;
    file = fopen(path.c_str(), "r");
#else
    file = fopen(("/tmp/" + tmpFileName).c_str(), "r");
#endif

    if (file == NULL)
        return;

    const int numberScanned = fscanf(file, "%d %d", &width, &height);

    if (numberScanned == 2 && width && height)
    {
        setSize(width, height);
    }

    fclose(file);
}

void WolfSpectrumUI::setParameterValueFeedback(uint32_t index, float value)
{
    setParameterValue(index, value);
    parameterChanged(index, value);
}

void WolfSpectrumUI::positionWidgets(uint width, uint height)
{
    fResizeHandle->setAbsolutePos(width - fResizeHandle->getWidth(), height - fResizeHandle->getHeight());
    fSpectrogram->setSize(width, height);
}

void WolfSpectrumUI::parameterChanged(uint32_t index, float value)
{
    value = std::round(value);

    switch (index)
    {
    case paramFrequencyScaling:
        fSpectrogram->setLogFrequencyScaling(value == WolfSpectrumPlugin::FrequencyScaling::FrequencyScalingLogarithmic);
        break;
    case paramBlockSize:
        fSpectrogram->setBlockSize(std::pow(2, 5 + (int)value)); //careful, this assumes block size 64 is the minimum
        break;
    case paramScrollDirection:
        fSpectrogram->setHorizontalScrolling(value == WolfSpectrumPlugin::ScrollDirection::ScrollDirectionHorizontal);
        break;
    case paramShowCaptions:
        fSpectrogram->setGridVisibility(value);
        break;
    case paramChannelMix:
        fSpectrogram->setChannelMix(value);
        break;
    case paramShowUIControls:
        fResizeHandle->setVisible(value);
        break;
    }

    fParameters[index] = value;
}

bool WolfSpectrumUI::onMouse(const MouseEvent &ev)
{
    if (ev.press && ev.button == 3) // right-click
    {
        fRightClickMenu->show(ev.pos.getX(), ev.pos.getY());

        return true;
    }

    return false;
}

void WolfSpectrumUI::rightClickMenuItemSelected(RightClickMenuItem *rightClickMenuItem)
{
    switch ((SpectrogramRightClickMenuItems)rightClickMenuItem->getId())
    {
    case SpectrogramRightClickMenuItems::FrequencyScalingLogarithmic:
        setParameterValueFeedback(paramFrequencyScaling, WolfSpectrumPlugin::FrequencyScalingLogarithmic);
        break;
    case SpectrogramRightClickMenuItems::FrequencyScalingLinear:
        setParameterValueFeedback(paramFrequencyScaling, WolfSpectrumPlugin::FrequencyScalingLinear);
        break;
    case SpectrogramRightClickMenuItems::ScrollDirectionVertical:
        setParameterValueFeedback(paramScrollDirection, WolfSpectrumPlugin::ScrollDirectionVertical);
        break;
    case SpectrogramRightClickMenuItems::ScrollDirectionHorizontal:
        setParameterValueFeedback(paramScrollDirection, WolfSpectrumPlugin::ScrollDirectionHorizontal);
        break;
    case SpectrogramRightClickMenuItems::BlockSize64:
        setParameterValueFeedback(paramBlockSize, WolfSpectrumPlugin::BlockSize64);
        break;
    case SpectrogramRightClickMenuItems::BlockSize128:
        setParameterValueFeedback(paramBlockSize, WolfSpectrumPlugin::BlockSize128);
        break;
    case SpectrogramRightClickMenuItems::BlockSize256:
        setParameterValueFeedback(paramBlockSize, WolfSpectrumPlugin::BlockSize256);
        break;
    case SpectrogramRightClickMenuItems::BlockSize512:
        setParameterValueFeedback(paramBlockSize, WolfSpectrumPlugin::BlockSize512);
        break;
    case SpectrogramRightClickMenuItems::BlockSize1024:
        setParameterValueFeedback(paramBlockSize, WolfSpectrumPlugin::BlockSize1024);
        break;
    case SpectrogramRightClickMenuItems::BlockSize2048:
        setParameterValueFeedback(paramBlockSize, WolfSpectrumPlugin::BlockSize2048);
        break;
    case SpectrogramRightClickMenuItems::BlockSize4096:
        setParameterValueFeedback(paramBlockSize, WolfSpectrumPlugin::BlockSize4096);
        break;
    case SpectrogramRightClickMenuItems::BlockSize8192:
        setParameterValueFeedback(paramBlockSize, WolfSpectrumPlugin::BlockSize8192);
        break;
    case SpectrogramRightClickMenuItems::BlockSize16384:
        setParameterValueFeedback(paramBlockSize, WolfSpectrumPlugin::BlockSize16384);
        break;
    case SpectrogramRightClickMenuItems::ChannelMixLRMean:
        setParameterValueFeedback(paramChannelMix, WolfSpectrumPlugin::ChannelMixLRMean);
        break;
    case SpectrogramRightClickMenuItems::ChannelMixL:
        setParameterValueFeedback(paramChannelMix, WolfSpectrumPlugin::ChannelMixL);
        break;
    case SpectrogramRightClickMenuItems::ChannelMixR:
        setParameterValueFeedback(paramChannelMix, WolfSpectrumPlugin::ChannelMixR);
        break;
    case SpectrogramRightClickMenuItems::ToggleCaptions:
        setParameterValueFeedback(paramShowCaptions, (float)!fParameters[paramShowCaptions]);
        break;
    case SpectrogramRightClickMenuItems::ToggleUIControls:
        setParameterValueFeedback(paramShowUIControls, (float)!fParameters[paramShowUIControls]);
        break;
    default:
        DISTRHO_SAFE_ASSERT_BREAK(false);
    }
}

void WolfSpectrumUI::onNanoDisplay()
{
    const float width = getWidth();
    const float height = getHeight();
}

void WolfSpectrumUI::uiIdle()
{
    repaint();
}

void WolfSpectrumUI::uiReshape(uint width, uint height)
{
    positionWidgets(width, height);
}

bool WolfSpectrumUI::onKeyboard(const KeyboardEvent &ev)
{
    if (ev.press)
    {
        if (ev.key == 95) //F11
        {
            fprintf(stderr, "Toggling fullscreen...\n");
            getParentWindow().toggleFullscreen();

            return true;
        }
        else if (ev.key == 96) //F12
        {
            WolfSpectrumConfig::load();
            fSpectrogram->clear();

            return true;
        }
        else if (ev.key == 9) //escape
        {
            getParentWindow().close();

            return true;
        }
    }

    return false;
}

void WolfSpectrumUI::resizeHandleMoved(int width, int height)
{
    setSize(width, height);
}

void WolfSpectrumUI::sampleRateChanged(const double sampleRate)
{
    fSpectrogram->setSampleRate(sampleRate);
}

UI *createUI()
{
    return new WolfSpectrumUI();
}

END_NAMESPACE_DISTRHO