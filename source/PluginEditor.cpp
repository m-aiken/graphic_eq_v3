#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "utils/colourpalette.h"

//==============================================================================
GraphicEqEditor::GraphicEqEditor (GraphicEqProcessor& p)
: AudioProcessorEditor (&p),
  processorRef (p),
  spectrumAnalyzer(processorRef.getSampleRate(), processorRef.lScsf, processorRef.rScsf, processorRef.apvts)
{
    addAndMakeVisible(spectrumAnalyzer);

    setSize (800, 500);
}

GraphicEqEditor::~GraphicEqEditor()
{
}

//==============================================================================
void GraphicEqEditor::paint (juce::Graphics& g)
{
    g.fillAll(ColourPalette::getColour(ColourPalette::Eggshell));
}

void GraphicEqEditor::resized()
{
    auto bounds = getLocalBounds();
    auto mainWindowWidth = bounds.getWidth();
    auto mainWindowHeight = bounds.getHeight();
    spectrumAnalyzer.setBounds(bounds.withSizeKeepingCentre(mainWindowWidth * 0.9, mainWindowHeight * 0.9));
}
