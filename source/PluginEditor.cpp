#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "utils/colourpalette.h"

//==============================================================================
GraphicEqEditor::GraphicEqEditor (GraphicEqProcessor& p)
: AudioProcessorEditor (&p),
  processorRef (p),
  spectrumAnalyzer(processorRef.getSampleRate(), processorRef.lScsf, processorRef.rScsf, processorRef.apvts),
  responseCurve(processorRef.apvts, processorRef.getSampleRate()),
  analyzerControls(processorRef.apvts),
  eqControls(processorRef.apvts)
{
    setLookAndFeel(&lnf);

    addAndMakeVisible(spectrumAnalyzer);
    addAndMakeVisible(responseCurve);
//    addAndMakeVisible(analyzerControls);
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

    spectrumAnalyzer.setBounds(padding,
                               padding,
                               mainWindowWidth - (padding * 2),
                               static_cast<int>(std::floor((mainWindowHeight * 0.7) - padding)));

    responseCurve.setBounds(spectrumAnalyzer.getBounds());

    auto tempEqControlsWidth = 732; // current fftBoundingBox width
    eqControls.setBounds(bounds.getCentreX() - (tempEqControlsWidth * 0.5),
                         spectrumAnalyzer.getBottom() + padding,
                         tempEqControlsWidth,
                         bounds.getBottom() - spectrumAnalyzer.getBottom() - (padding * 2));

    /*
    analyzerControls.setBounds(padding,
                               spectrumAnalyzer.getBottom() + padding,
                               mainWindowWidth / 3,
                               bounds.getBottom() - spectrumAnalyzer.getBottom() - (padding * 2));
    */
}
