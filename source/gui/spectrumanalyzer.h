#pragma once

#include <JuceHeader.h>

#include "pathproducer.h"
#include "paramlistener.h"
#include "../dsp/singlesamplefifo.h"
#include "../utils/globals.h"

//==============================================================================
struct SpectrumGrid : juce::Component
{
    void paint(juce::Graphics& g) override;
};

//==============================================================================
struct SpectrumAnalyzer : juce::Component, juce::Timer
{
    SpectrumAnalyzer(double _sampleRate,
                     SingleChannelSampleFifo<juce::AudioBuffer<float>>& leftScsf,
                     SingleChannelSampleFifo<juce::AudioBuffer<float>>& rightScsf,
                     juce::AudioProcessorValueTreeState& apvts);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

private:
    double sampleRate;
    SpectrumGrid backgroundGrid;

    juce::Path leftAnalyzerPath, rightAnalyzerPath;

    PathProducer leftPathProducer, rightPathProducer;

//    bool active { false };
    bool active { true }; // TODO for test only

    void setActive(bool activeState);
    void updateDecayRate(float decayRate);
    void updateOrder(float value);
    void animate();

    std::unique_ptr<ParamListener<float>> analyzerOrderParamListener;
    std::unique_ptr<ParamListener<float>> analyzerDecayRateParamListener;
};
