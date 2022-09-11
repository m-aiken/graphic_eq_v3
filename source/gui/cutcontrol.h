#pragma once

#include <JuceHeader.h>
#include "customrotarycontrol.h"
#include "../dsp/filterutils.h"

//==============================================================================
struct CutControl : juce::Component
{
    CutControl(juce::AudioProcessorValueTreeState& apvts, const FilterUtils::ChainPositions& _chainPosition);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    FilterUtils::ChainPositions chainPosition;

    std::unique_ptr<CustomRotaryControl> freqSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment;
};
