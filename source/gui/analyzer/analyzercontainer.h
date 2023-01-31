#pragma once

#include "../../dsp/monobufferfifo.h"
#include "JuceHeader.h"
#include "responsecurve.h"
#include "spectrumanalyzer.h"
#include "spectrumgrid.h"

//==============================================================================
struct AnalyzerContainer : juce::Component
{
    AnalyzerContainer(juce::AudioProcessorValueTreeState&       apvts,
                      double                                    sampleRate,
                      MonoBufferFifo<juce::AudioBuffer<float>>& preEqBufferL,
                      MonoBufferFifo<juce::AudioBuffer<float>>& preEqBufferR,
                      MonoBufferFifo<juce::AudioBuffer<float>>& postEqBufferL,
                      MonoBufferFifo<juce::AudioBuffer<float>>& postEqBufferR);

    void paint(juce::Graphics& g) override;
    void resized() override;

    size_t getActiveNodeIndex() { return responseCurve.getActiveNodeIndex(); }

private:
    SpectrumGrid     backgroundGrid;
    SpectrumAnalyzer spectrumAnalyzerPreEq;
    SpectrumAnalyzer spectrumAnalyzerPostEq;
    ResponseCurve    responseCurve;
};
