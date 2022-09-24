#pragma once

#include "JuceHeader.h"

//==============================================================================
struct AnalyzerOverlay : juce::Component
{
    void paint(juce::Graphics& g) override;
    void mouseDrag(const juce::MouseEvent& event) override;
};
