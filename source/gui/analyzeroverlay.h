#pragma once

#include <JuceHeader.h>
#include "analyzerbase.h"

//==============================================================================
struct AnalyzerOverlay : AnalyzerBase
{
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDrag(const juce::MouseEvent& event) override;
};
