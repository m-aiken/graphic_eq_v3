#pragma once

#include "JuceHeader.h"
#include "peakcontrol.h"
#include "cutcontrol.h"
#include "../../utils/globals.h"

//==============================================================================
struct EqControlsContainer : juce::Component
{
    EqControlsContainer(juce::AudioProcessorValueTreeState& apvts);
    void resized() override;

    void setBandHasNodeSelection(size_t activeNodeIndex);
private:
    CutControl lowCutControl;
    CutControl highCutControl;

    std::array<std::unique_ptr<PeakControl>, Globals::getNumPeakBands()> peakControls;
};
