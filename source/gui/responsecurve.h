#pragma once

#include <JuceHeader.h>
#include "analyzerbase.h"
#include "../dsp/filterUtils.h"

//==============================================================================
struct ResponseCurve : AnalyzerBase, juce::AudioProcessorParameter::Listener, juce::Timer
{
    ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate);
    ~ResponseCurve();

    void paint(juce::Graphics& g) override;
    void updateMonoChain();

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}

    void timerCallback() override;

    void addListeners();
    void removeListeners();

private:
    juce::AudioProcessorValueTreeState& apvts;

    FilterUtils::MonoChain monoChain;

    double sampleRate;

    juce::Atomic<bool> parametersChanged { false };
};
