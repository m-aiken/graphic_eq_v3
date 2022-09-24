#include "analyzercontainer.h"

//==============================================================================
AnalyzerContainer::AnalyzerContainer(juce::AudioProcessorValueTreeState& apvts,
                                     double sampleRate,
                                     SingleChannelSampleFifo<juce::AudioBuffer<float>>& leftScsf,
                                     SingleChannelSampleFifo<juce::AudioBuffer<float>>& rightScsf)
: spectrumAnalyzer(sampleRate, leftScsf, rightScsf, apvts),
  responseCurve(apvts, sampleRate)
{
    addAndMakeVisible(spectrumAnalyzer);
    addAndMakeVisible(responseCurve);
    addAndMakeVisible(analyzerOverlay);
}

void AnalyzerContainer::resized()
{
    auto bounds = getLocalBounds();
    spectrumAnalyzer.setBounds(bounds);
    responseCurve.setBounds(bounds);
    analyzerOverlay.setBounds(bounds);
}
