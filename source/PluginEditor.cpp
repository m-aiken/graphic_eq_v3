#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "utils/colourpalette.h"

//==============================================================================
GraphicEqEditor::GraphicEqEditor (GraphicEqProcessor& p)
: AudioProcessorEditor (&p),
  processorRef (p),
  spectrumAnalyzer(processorRef.getSampleRate(), processorRef.lScsf, processorRef.rScsf, processorRef.apvts),
  analyzerControls(processorRef.apvts)
{
    setLookAndFeel(&lnf);

    addAndMakeVisible(spectrumAnalyzer);
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
    /*
    auto bounds = getLocalBounds();
    auto mainWindowWidth = bounds.getWidth();
    auto mainWindowHeight = bounds.getHeight();
    auto padding = 10;

    auto spectrumAnalyzerBounds = juce::Rectangle<int>(padding,
                                                       padding,
                                                       mainWindowWidth - (padding * 2),
                                                       static_cast<int>(std::floor((mainWindowHeight * 0.7) - padding)));

    g.setColour(ColourPalette::getColour(ColourPalette::Blue));
    g.drawRect(spectrumAnalyzerBounds, 1);
    */
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

    analyzerControls.setBounds(padding,
                               spectrumAnalyzer.getBottom() + padding,
                               mainWindowWidth / 3,
                               bounds.getBottom() - spectrumAnalyzer.getBottom() - (padding * 2));
}
