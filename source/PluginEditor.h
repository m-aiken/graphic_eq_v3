#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "gui/analyzer/analyzercontainer.h"
#include "gui/analyzercontrols.h"
#include "gui/bandcontrols/eqcontrolscontainer.h"
#include "gui/customlookandfeel.h"
#include "gui/widgets/themebutton.h"

//==============================================================================
class GraphicEqEditor : public juce::AudioProcessorEditor, juce::Timer
{
public:
    explicit GraphicEqEditor(GraphicEqProcessor&);
    ~GraphicEqEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    void drawDbLabels(juce::Graphics& g, juce::Rectangle<int>& labelBounds);
    void drawFrequencyLabels(juce::Graphics& g, juce::Rectangle<int>& labelBounds);

    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GraphicEqProcessor& processorRef;

    AnalyzerControls    analyzerControls;
    ThemeButton         themeButton;
    AnalyzerContainer   analyzerContainer;
    EqControlsContainer eqControls;

    CustomLookAndFeel lnf;

    size_t activeNode;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphicEqEditor)
};
