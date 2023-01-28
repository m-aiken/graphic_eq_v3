#pragma once

#include "JuceHeader.h"

#include "../../dsp/singlesamplefifo.h"
#include "../../utils/globals.h"
#include "../paramlistener.h"
#include "../pathproducer.h"

//==============================================================================
struct SpectrumAnalyzer : juce::Component, juce::Timer
{
    SpectrumAnalyzer(double                                             _sampleRate,
                     SingleChannelSampleFifo<juce::AudioBuffer<float>>& leftScsf,
                     SingleChannelSampleFifo<juce::AudioBuffer<float>>& rightScsf,
                     juce::AudioProcessorValueTreeState&                apvts);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

private:
    double       sampleRate;
    juce::Path   leftAnalyzerPath;
    juce::Path   rightAnalyzerPath;
    PathProducer leftPathProducer;
    PathProducer rightPathProducer;
    //    bool         active { false };
    bool active { true }; // TODO for test only

    void setActive(bool activeState);
    void updateDecayRate(float decayRate);
    void updateOrder(float value);
    void animate();

    std::unique_ptr<ParamListener<float>> analyzerOrderParamListener;
    std::unique_ptr<ParamListener<float>> analyzerDecayRateParamListener;
};
