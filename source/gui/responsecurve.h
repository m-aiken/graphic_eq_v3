#pragma once

#include <JuceHeader.h>
#include "../dsp/filterUtils.h"

//==============================================================================
struct ResponseCurve : juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer
{
    ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate);
    ~ResponseCurve();

    void paint(juce::Graphics& g) override;
    void updateMonoChain();

    // Parameter Listener pure virtuals
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}

    // Timer pure virtual
    void timerCallback() override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    FilterUtils::MonoChain monoChain;

    double sampleRate;

    juce::Atomic<bool> parametersChanged { false };
};
