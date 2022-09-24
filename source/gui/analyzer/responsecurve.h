#pragma once

#include "JuceHeader.h"
#include "responsecurvenodes.h"
#include "../../dsp/filterutils.h"
#include "../../dsp/peakband.h"
#include "../../utils/globals.h"

//==============================================================================
struct ResponseCurve : juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer
{
    ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate);
    ~ResponseCurve();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void updateMonoChain();
    std::vector<double> getMagnitudes(int boundsWidth);

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}

    void timerCallback() override;

    void addListeners();
    void removeListeners();

private:
    double             sampleRate;
    juce::Atomic<bool> parametersChanged { false };

    juce::AudioProcessorValueTreeState& apvts;
    FilterUtils::MonoChain              monoChain;
    juce::AudioParameterFloat*          lowCutFreqParam   { nullptr };
    juce::AudioParameterChoice*         lowCutSlopeParam  { nullptr };
    juce::AudioParameterFloat*          highCutFreqParam  { nullptr };
    juce::AudioParameterChoice*         highCutSlopeParam { nullptr };

    std::array<PeakBand, Globals::getNumPeakBands()> peakBands;
    ResponseCurveNodes                               peakNodes;
};
