#pragma once

#include "JuceHeader.h"
#include "../../dsp/filterutils.h"
#include "../../dsp/peakband.h"
#include "../../utils/globals.h"
#include "../../utils/colourpalette.h"

//==============================================================================
struct Node : juce::Component
{
    void paint(juce::Graphics& g) override
    {
        g.setColour(ColourPalette::getColour(ColourPalette::Salmon));
        g.fillEllipse(getLocalBounds().toFloat());
    }
};

//==============================================================================
struct ResponseCurve : juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer
{
    ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate);
    ~ResponseCurve();

    void paint(juce::Graphics& g) override;

    void updateMonoChain();
    std::vector<double> getMagnitudes(int boundsWidth);

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}

    void timerCallback() override;

    void addListeners();
    void removeListeners();

private:
    double sampleRate;
    int    nodeDiameter { 12 };
    int    nodeRadius { 6 };

    juce::Atomic<bool> parametersChanged { false };

    juce::AudioProcessorValueTreeState& apvts;
    FilterUtils::MonoChain              monoChain;
    juce::AudioParameterFloat*          lowCutFreqParam   { nullptr };
    juce::AudioParameterChoice*         lowCutSlopeParam  { nullptr };
    juce::AudioParameterFloat*          highCutFreqParam  { nullptr };
    juce::AudioParameterChoice*         highCutSlopeParam { nullptr };

    std::array<PeakBand, Globals::getNumPeakBands()> peakBands;
    std::array<Node, Globals::getNumPeakBands()>     peakNodes;
//    ResponseCurveNodes                               peakNodes;
};
