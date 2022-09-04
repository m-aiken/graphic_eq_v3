#pragma once

#include <JuceHeader.h>
#include "paramlistener.h"
#include "../dsp/filterUtils.h"

//==============================================================================
struct ResponseCurve : juce::Component
{
    ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate);

    void paint(juce::Graphics& g) override;
    void updateMonoChain();

private:
    juce::AudioProcessorValueTreeState& apvts;

    std::unique_ptr<ParamListener<float>> lowCutFreqListener;
    std::unique_ptr<ParamListener<float>> lowCutSlopeListener;

    FilterUtils::MonoChain monoChain;

    double sampleRate;

    juce::AudioParameterFloat*  lowCutFreqParam   { nullptr };
    juce::AudioParameterChoice* lowCutSlopeParam  { nullptr };
    juce::AudioParameterFloat*  highCutFreqParam  { nullptr };
    juce::AudioParameterChoice* highCutSlopeParam { nullptr };
    juce::AudioParameterFloat*  peakFreqParam     { nullptr };
    juce::AudioParameterFloat*  peakGainParam     { nullptr };
    juce::AudioParameterFloat*  peakQParam        { nullptr };
};
