#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "utils/colourpalette.h"

//==============================================================================
GraphicEqEditor::GraphicEqEditor (GraphicEqProcessor& p)
: AudioProcessorEditor (&p),
  processorRef (p),
  spectrumAnalyzer(processorRef.getSampleRate(), processorRef.lScsf, processorRef.rScsf, processorRef.apvts),
  responseCurve(processorRef.apvts, processorRef.getSampleRate()),
  analyzerControls(processorRef.apvts)
{
    setLookAndFeel(&lnf);

    addAndMakeVisible(spectrumAnalyzer);
    addAndMakeVisible(responseCurve);
    addAndMakeVisible(analyzerControls);

    setSize (800, 600);
}

GraphicEqEditor::~GraphicEqEditor()
{
    setLookAndFeel(nullptr);
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
    auto padding = 10;

    spectrumAnalyzer.setBounds(padding,
                               padding,
                               mainWindowWidth - (padding * 2),
                               static_cast<int>(std::floor((mainWindowHeight * 0.7) - padding)));

    responseCurve.setBounds(spectrumAnalyzer.getBounds());

    analyzerControls.setBounds(padding,
                               spectrumAnalyzer.getBottom() + padding,
                               mainWindowWidth / 3,
                               bounds.getBottom() - spectrumAnalyzer.getBottom() - (padding * 2));
}
