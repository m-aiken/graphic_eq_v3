#pragma once

#include "JuceHeader.h"
#include "spectrumgrid.h"
#include "spectrumanalyzer.h"
#include "responsecurve.h"
#include "../../dsp/singlesamplefifo.h"

//==============================================================================
struct AnalyzerContainer : juce::Component
{
    AnalyzerContainer(juce::AudioProcessorValueTreeState& apvts,
                      double sampleRate,
                      SingleChannelSampleFifo<juce::AudioBuffer<float>>& leftScsf,
                      SingleChannelSampleFifo<juce::AudioBuffer<float>>& rightScsf);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    SpectrumGrid     backgroundGrid;
    SpectrumAnalyzer spectrumAnalyzer;
    ResponseCurve    responseCurve;
};
