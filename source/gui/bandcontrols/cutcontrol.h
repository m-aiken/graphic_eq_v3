#pragma once

#include "JuceHeader.h"
#include "../widgets/powerbutton.h"
#include "../widgets/customrotarycontrol.h"
#include "../widgets/customlinearslider.h"
#include "../../dsp/filterutils.h"

//==============================================================================
struct CutControl : juce::Component
{
    CutControl(juce::AudioProcessorValueTreeState& apvts, const FilterUtils::ChainPositions& _chainPosition);
    void paint(juce::Graphics& g) override;
    void resized() override;

    void setEnablement();

private:
    FilterUtils::ChainPositions chainPosition;

    std::unique_ptr<PowerButton> powerButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;

    std::unique_ptr<CustomRotaryControl> freqSlider;
    std::unique_ptr<CustomLinearSlider>  slopeSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> slopeAttachment;
};
