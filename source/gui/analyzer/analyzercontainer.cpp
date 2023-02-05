#include "analyzercontainer.h"
#include "../../utils/colourpalette.h"
#include "../../utils/globals.h"

//==============================================================================
AnalyzerContainer::AnalyzerContainer(juce::AudioProcessorValueTreeState&       apvts,
                                     double                                    sampleRate,
                                     MonoBufferFifo<juce::AudioBuffer<float>>& preEqBufferL,
                                     MonoBufferFifo<juce::AudioBuffer<float>>& preEqBufferR,
                                     MonoBufferFifo<juce::AudioBuffer<float>>& postEqBufferL,
                                     MonoBufferFifo<juce::AudioBuffer<float>>& postEqBufferR)
    : spectrumAnalyzerPreEq(sampleRate, preEqBufferL, preEqBufferR, apvts, ColourPalette::getColourV2(ColourPalette::AnalyzerPreEq).withAlpha(0.1f))
    , spectrumAnalyzerPostEq(sampleRate, postEqBufferL, postEqBufferR, apvts, ColourPalette::getColourV2(ColourPalette::AnalyzerPostEq).withAlpha(0.25f))
    , responseCurve(apvts, sampleRate)
{
    addAndMakeVisible(backgroundGrid);
    addAndMakeVisible(spectrumAnalyzerPreEq);
    addAndMakeVisible(spectrumAnalyzerPostEq);
    addAndMakeVisible(responseCurve);
}

void AnalyzerContainer::paint(juce::Graphics& g)
{
    // TODO make a decision on this fill
    //    g.fillAll(ColourPalette::getColour(ColourPalette::Salmon).withAlpha(0.1f));
}

void AnalyzerContainer::resized()
{
    auto bounds = getLocalBounds();
    backgroundGrid.setBounds(bounds);
    spectrumAnalyzerPreEq.setBounds(bounds);
    spectrumAnalyzerPostEq.setBounds(bounds);
    responseCurve.setBounds(bounds);
}
