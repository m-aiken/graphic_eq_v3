#pragma once

#include <JuceHeader.h>
#include "customlinearslider.h"
#include "customrotarycontrol.h"

//==============================================================================
struct AnalyzerControls : juce::Component
{
    AnalyzerControls(juce::AudioProcessorValueTreeState& apvts);
    void paint(juce::Graphics& g) override;
    void resized() override;
private:
    std::unique_ptr<CustomRotaryControl> decayRateSlider;
    std::unique_ptr<CustomLinearSlider> fftOrderSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fftOrderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayRateAttachment;
};
