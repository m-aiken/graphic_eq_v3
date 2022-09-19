#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "utils/colourpalette.h"

//==============================================================================
GraphicEqEditor::GraphicEqEditor (GraphicEqProcessor& p)
: AudioProcessorEditor (&p),
  processorRef (p),
  analyzerControls(processorRef.apvts),
  spectrumAnalyzer(processorRef.getSampleRate(), processorRef.lScsf, processorRef.rScsf, processorRef.apvts),
  responseCurve(processorRef.apvts, processorRef.getSampleRate()),
  eqControls(processorRef.apvts)
{
    setLookAndFeel(&lnf);

    addAndMakeVisible(analyzerControls);
    addAndMakeVisible(spectrumAnalyzer);
    addAndMakeVisible(responseCurve);
    addAndMakeVisible(eqControls);

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
    
    auto tempEqControlsWidth = 732; // current fftBoundingBox width
    analyzerControls.setBounds(bounds.getCentreX() - (tempEqControlsWidth * 0.5),
                               padding,
                               tempEqControlsWidth,
                               padding * 3);

    spectrumAnalyzer.setBounds(padding,
                               padding * 5,
                               mainWindowWidth - (padding * 2),
                               static_cast<int>(std::floor((mainWindowHeight * 0.6) - padding)));

    responseCurve.setBounds(spectrumAnalyzer.getBounds());

    eqControls.setBounds(bounds.getCentreX() - (tempEqControlsWidth * 0.5),
                         spectrumAnalyzer.getBottom() + padding,
                         tempEqControlsWidth,
                         bounds.getBottom() - spectrumAnalyzer.getBottom() - (padding * 2));
}
