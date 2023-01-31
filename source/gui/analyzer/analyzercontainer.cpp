#include "analyzercontainer.h"
#include "../../utils/colourpalette.h"
#include "../../utils/globals.h"

//==============================================================================
AnalyzerContainer::AnalyzerContainer(juce::AudioProcessorValueTreeState&       apvts,
                                     double                                    sampleRate,
                                     MonoBufferFifo<juce::AudioBuffer<float>>& leftScsf,
                                     MonoBufferFifo<juce::AudioBuffer<float>>& rightScsf)
    : spectrumAnalyzer(sampleRate, leftScsf, rightScsf, apvts)
    , responseCurve(apvts, sampleRate)
{
    addAndMakeVisible(backgroundGrid);
    addAndMakeVisible(spectrumAnalyzer);
    addAndMakeVisible(responseCurve);
}

void AnalyzerContainer::paint(juce::Graphics& g)
{
    g.fillAll(ColourPalette::getColour(ColourPalette::Salmon).withAlpha(0.1f));
}

void AnalyzerContainer::resized()
{
    auto bounds = getLocalBounds();
    backgroundGrid.setBounds(bounds);
    spectrumAnalyzer.setBounds(bounds);
    responseCurve.setBounds(bounds);
}
