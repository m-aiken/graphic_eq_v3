#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "gui/spectrumanalyzer.h"
#include "gui/customlookandfeel.h"
#include "gui/analyzercontrols.h"

//==============================================================================
class GraphicEqEditor : public juce::AudioProcessorEditor
{
public:
    explicit GraphicEqEditor (GraphicEqProcessor&);
    ~GraphicEqEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GraphicEqProcessor& processorRef;

    SpectrumAnalyzer spectrumAnalyzer;
    AnalyzerControls analyzerControls;

    CustomLookAndFeel lnf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphicEqEditor)
};
