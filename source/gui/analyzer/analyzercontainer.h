#pragma once

#include "JuceHeader.h"
#include "analyzeroverlay.h"
#include "responsecurve.h"
//#include "responsecurvenodes.h"
#include "spectrumanalyzer.h"
#include "../../dsp/singlesamplefifo.h"

//==============================================================================
struct AnalyzerContainer : juce::Component
{
    AnalyzerContainer(juce::AudioProcessorValueTreeState& apvts,
                      double sampleRate,
                      SingleChannelSampleFifo<juce::AudioBuffer<float>>& leftScsf,
                      SingleChannelSampleFifo<juce::AudioBuffer<float>>& rightScsf);

    void resized() override;

private:
    SpectrumAnalyzer spectrumAnalyzer;
    ResponseCurve    responseCurve;
    AnalyzerOverlay  analyzerOverlay;
};
