#pragma once

#include "JuceHeader.h"

//==============================================================================
struct CustomLinearSlider : juce::Slider
{
    CustomLinearSlider(juce::RangedAudioParameter& rap)
            : juce::Slider(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextEntryBoxPosition::NoTextBox), param(&rap) { }

    void paint(juce::Graphics& g) override;
private:
    juce::RangedAudioParameter* param;
};
