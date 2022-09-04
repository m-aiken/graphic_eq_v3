#pragma once

#include <JuceHeader.h>
#include "paramlistener.h"
#include "../dsp/filterUtils.h"

//==============================================================================
struct ResponseCurve : juce::Component
{
    ResponseCurve(juce::AudioProcessorValueTreeState& apvts, double _sampleRate);

    void paint(juce::Graphics& g) override;
    void updateMonoChain();

private:
    std::unique_ptr<ParamListener<float>> lowCutFreqListener;
    std::unique_ptr<ParamListener<float>> lowCutSlopeListener;

    FilterUtils::MonoChain monoChain;

    double sampleRate;
};
