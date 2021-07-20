#ifndef WOLF_SPECTRUM_CONFIG_H_INCLUDED
#define WOLF_SPECTRUM_CONFIG_H_INCLUDED

START_NAMESPACE_DISTRHO

#define CONFIG_NAMESPACE WolfSpectrumConfig
namespace CONFIG_NAMESPACE
{
extern bool isLoaded;

extern Color color_ramp_1;
extern Color color_ramp_2;
extern Color color_ramp_3;
extern Color color_ramp_4;
extern Color color_ramp_5;
extern Color color_ramp_6;
extern Color color_ramp_7;
extern Color color_ramp_8;
extern Color color_ramp_9;
extern Color color_ramp_10;
extern Color right_click_menu_border_color;

void load();
}

END_NAMESPACE_DISTRHO

#endif