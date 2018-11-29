#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include "src/DistrhoDefines.h"

#if defined(DISTRHO_OS_WINDOWS)
#include <windows.h>
#include <shlobj.h>
#else
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#endif

#include "Color.hpp"
#include "INIReader.h"
#include "StringManipulation.hpp"

START_NAMESPACE_DISTRHO

namespace WolfSpectrumConfig
{

enum ColorType
{
    kColorTypeRGB = 0,
    kColorTypeRGBA,
    kColorTypeHSL,
    kColorTypeHSLA
};

bool isLoaded = false;

Color color_ramp_1 = Color(252, 243, 178);
Color color_ramp_2 = Color(255, 193, 105);
Color color_ramp_3 = Color(255, 114, 54);
Color color_ramp_4 = Color(242, 32, 33);
Color color_ramp_5 = Color(185, 10, 82);
Color color_ramp_6 = Color(126, 4, 116);
Color color_ramp_7 = Color(62, 2, 99);
Color color_ramp_8 = Color(33, 2, 83);
Color color_ramp_9 = Color(11, 1, 48);
Color color_ramp_10 = Color(0, 0, 0, 0);

static std::string getSystemWideConfigPath()
{
#if defined(DISTRHO_OS_WINDOWS)
    return getLocalConfigPath(); //pretty sure Windows users don't care about this
#else
    return "/etc/wolf-spectrum.conf";
#endif
}

static std::string getLocalConfigPath()
{
    const std::string configName = "wolf-spectrum.conf";

#if defined(DISTRHO_OS_WINDOWS)
    CHAR my_documents[MAX_PATH];
    HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

    if (result != S_OK)
        return "";

    return std::string(my_documents) + "\\" + configName;
#else
    const char *homeDirectory;

    if ((homeDirectory = getenv("HOME")) == NULL)
    {
        homeDirectory = getpwuid(getuid())->pw_dir;
    }

    std::string fileLocation;

#if defined(DISTRHO_OS_MAC)
    fileLocation = "/Library/Application Support/";
#else
    fileLocation = "/.config/";
#endif
    return homeDirectory + fileLocation + configName;
#endif
}

/**
 * Convert a string containing color values to a Color struct.
 * Supported formats: rgb(rgb), rgba(r,g,b,a), hsl(h,s,l), hsla(h,s,l,a)
 */
static void colorFromString(std::string colorStr, Color *targetColor)
{
    if (colorStr == "")
    {
        return;
    }

    const char *str = colorStr.c_str();
    char const *rest;

    std::string colorTypeString = wolf::takeUntil(str, '(');
    ColorType colorType;

    unsigned char x = 255, y = 255, z = 255, a = 255;

    int scanStatus;

    if (colorTypeString == "rgb")
    {
        scanStatus = sscanf(str, " rgb ( %hhu , %hhu , %hhu ) ", &x, &y, &z);
        colorType = kColorTypeRGB;
    }
    else if (colorTypeString == "hsl")
    {
        scanStatus = sscanf(str, " hsl ( %hhu , %hhu , %hhu ) ", &x, &y, &z);
        colorType = kColorTypeHSL;
    }
    else if (colorTypeString == "rgba")
    {
        scanStatus = sscanf(str, " rgba ( %hhu , %hhu , %hhu , %hhu ) ", &x, &y, &z, &a);
        colorType = kColorTypeRGBA;
    }
    else if (colorTypeString == "hsla")
    {
        scanStatus = sscanf(str, " hsla ( %hhu , %hhu , %hhu , %hhu ) ", &x, &y, &z, &a);
        colorType = kColorTypeHSLA;
    }
    else
    {
        fprintf(stderr, "wolf-spectrum: Warning! Invalid color type in config file: %s.\n", colorStr.c_str());

        return;
    }

    if (scanStatus == 3 || scanStatus == 4)
    {
        if (colorType == kColorTypeRGB || colorType == kColorTypeRGBA)
            *targetColor = Color(x, y, z, a);
        else
            *targetColor = Color::fromHSL(x / 255.f, y / 255.f, z / 255.f, a / 255.f);
    }
    else
    {
        fprintf(stderr, "wolf-spectrum: Warning! Color has an invalid number of arguments: %s.\n", colorStr.c_str());
    }

    return;
}

void load()
{
    INIReader reader(getLocalConfigPath());

    if (reader.ParseError() < 0)
    {
        reader = INIReader(getSystemWideConfigPath());

        if (reader.ParseError() < 0)
        {
            std::cout << "Can't load 'wolf-spectrum.conf', using defaults\n";
            return;
        }
    }

    colorFromString(reader.Get("colors", "color_ramp_1", ""), &color_ramp_1);
    colorFromString(reader.Get("colors", "color_ramp_2", ""), &color_ramp_2);
    colorFromString(reader.Get("colors", "color_ramp_3", ""), &color_ramp_3);
    colorFromString(reader.Get("colors", "color_ramp_4", ""), &color_ramp_4);
    colorFromString(reader.Get("colors", "color_ramp_5", ""), &color_ramp_5);
    colorFromString(reader.Get("colors", "color_ramp_6", ""), &color_ramp_6);
    colorFromString(reader.Get("colors", "color_ramp_7", ""), &color_ramp_7);
    colorFromString(reader.Get("colors", "color_ramp_8", ""), &color_ramp_8);
    colorFromString(reader.Get("colors", "color_ramp_9", ""), &color_ramp_9);
    colorFromString(reader.Get("colors", "color_ramp_10", ""), &color_ramp_10);

    isLoaded = true;
    std::cout << "Config loaded from 'wolf-spectrum.conf'\n";
}
}

END_NAMESPACE_DISTRHO