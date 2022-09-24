#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "gui/analyzer/analyzercontainer.h"
#include "gui/customlookandfeel.h"
#include "gui/analyzercontrols.h"
#include "gui/eqcontrolscontainer.h"

//==============================================================================
class GraphicEqEditor : public juce::AudioProcessorEditor
{
public:
    explicit GraphicEqEditor (GraphicEqProcessor&);
    ~GraphicEqEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void drawDbLabels(juce::Graphics& g, juce::Rectangle<int>& labelBounds);
    void drawFrequencyLabels(juce::Graphics& g, juce::Rectangle<int>& labelBounds);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GraphicEqProcessor& processorRef;
    
    AnalyzerControls    analyzerControls;
    AnalyzerContainer   analyzerContainer;
    EqControlsContainer eqControls;

    CustomLookAndFeel lnf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphicEqEditor)
};
