#pragma once

#include "JuceHeader.h"
#include "peakcontrol.h"
#include "cutcontrol.h"

//==============================================================================
struct EqControlsContainer : juce::Component
{
    EqControlsContainer(juce::AudioProcessorValueTreeState& apvts);
    void resized() override;
private:
    CutControl lowCutControl;
    CutControl highCutControl;
    
    PeakControl peakControl0;
    PeakControl peakControl1;
    PeakControl peakControl2;
    PeakControl peakControl3;
    PeakControl peakControl4;
    PeakControl peakControl5;
};
