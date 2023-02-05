#pragma once

#include "JuceHeader.h"

//==============================================================================
struct ThemeButton : juce::ToggleButton
{
    ThemeButton();
    void          paint(juce::Graphics& g) override;
    DrawableImage icon;
};
