#pragma once

#include <JuceHeader.h>
#include "analyzerbase.h"
#include "responsecurvenode.h"
#include "../dsp/filterUtils.h"

//==============================================================================
struct ResponseCurve : AnalyzerBase, juce::AudioProcessorParameter::Listener, juce::Timer
{
    ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate);
    ~ResponseCurve();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void updateMonoChain();
    void updateNodeCoordinates(ResponseCurveNode& node, juce::AudioParameterFloat* freqParam, juce::AudioParameterFloat* gainParam);

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

    ResponseCurveNode peakNode;
    int nodeDiameter { 16 };

    juce::AudioParameterFloat*  lowCutFreqParam   { nullptr };
    juce::AudioParameterChoice* lowCutSlopeParam  { nullptr };

    juce::AudioParameterFloat*  highCutFreqParam  { nullptr };
    juce::AudioParameterChoice* highCutSlopeParam { nullptr };

    juce::AudioParameterFloat*  peakFreqParam     { nullptr };
    juce::AudioParameterFloat*  peakGainParam     { nullptr };
    juce::AudioParameterFloat*  peakQParam        { nullptr };
};
