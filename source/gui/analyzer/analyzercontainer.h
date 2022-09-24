#pragma once

#include "JuceHeader.h"
#include "spectrumgrid.h"
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

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDrag(const juce::MouseEvent& event) override;

private:
    SpectrumGrid     backgroundGrid;
    SpectrumAnalyzer spectrumAnalyzer;
    ResponseCurve    responseCurve;
};
