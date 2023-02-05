#include "themebutton.h"

//==============================================================================
ThemeButton::ThemeButton()
{
    icon.setImage(juce::ImageFileFormat::loadFrom(BinaryData::day_and_night_icon_png, BinaryData::day_and_night_icon_pngSize));
}

void ThemeButton::paint(juce::Graphics& g)
{
    icon.drawWithin(g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, 0.7f);
}
