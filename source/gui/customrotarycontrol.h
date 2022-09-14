#pragma once

#include <JuceHeader.h>

//==============================================================================
struct CustomRotaryControl : juce::Slider
{
    CustomRotaryControl(juce::RangedAudioParameter& rap, const juce::String& unitSuffix, const juce::String& title);

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getRotaryBounds() const;
    int getTextHeight() const { return 10; }
    juce::String getDisplayString() const;
    void updateParam(juce::RangedAudioParameter* newParam) { param = newParam; }
private:
    juce::RangedAudioParameter* param;
    juce::String suffix;
};
