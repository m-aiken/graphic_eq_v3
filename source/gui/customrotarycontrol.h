#pragma once

#include <JuceHeader.h>

//==============================================================================
struct CustomRotaryControl : juce::Slider
{
    CustomRotaryControl(juce::RangedAudioParameter& rap, const juce::String& unitSuffix, const juce::String& title);

    struct LabelPos
    {
        float pos;
        juce::String label;
    };

    juce::Array<LabelPos> labels;

    void setLabels();
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getRotaryBounds() const;
    int getTextHeight() const { return 13; }
    juce::String getDisplayString() const;
    void updateParam(juce::RangedAudioParameter* newParam) { param = newParam; }
private:
    juce::RangedAudioParameter* param;
    juce::String suffix;
};
