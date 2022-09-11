#pragma once

#include <JuceHeader.h>
#include "customrotarycontrol.h"

//==============================================================================
struct PeakControl : juce::Component
{
    PeakControl(juce::AudioProcessorValueTreeState& apvts, const int _bandNum);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    int bandNum;

    std::unique_ptr<CustomRotaryControl> freqSlider;
    std::unique_ptr<CustomRotaryControl> gainSlider;
    std::unique_ptr<CustomRotaryControl> qSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qAttachment;
};
