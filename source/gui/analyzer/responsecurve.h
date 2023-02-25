#pragma once

#include "../../dsp/filterutils.h"
#include "../../dsp/peakband.h"
#include "../../utils/colourpalette.h"
#include "../../utils/globals.h"
#include "JuceHeader.h"

//==============================================================================
struct Node : juce::ToggleButton
{
    Node(juce::AudioProcessorValueTreeState& _apvts, int _bandNum);
    void paint(juce::Graphics& g) override;

private:
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> paramAttachment;
};

//==============================================================================
struct ResponseCurve : juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer
{
    ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate);
    ~ResponseCurve();

    void paint(juce::Graphics& g) override;
    void mouseDrag(const juce::MouseEvent& event) override;

    void                               updateMonoChain();
    std::array<std::vector<double>, 9> getMagnitudes(int boundsWidth);

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override { }

    void timerCallback() override;

    void addListeners();
    void removeListeners();
    bool allControlPtrsValid();

    std::array<juce::Point<int>, Globals::getNumPeakBands()>& getNodeCoordinates() { return nodeCoordinates; }

    size_t getActiveNodeIndex() { return activeNode; }

private:
    double sampleRate;
    int    nodeDiameter { 6 };
    int    nodeRadius { 3 };

    juce::Atomic<bool> parametersChanged { false };

    juce::AudioProcessorValueTreeState& apvts;
    FilterUtils::MonoChain              monoChain;

    juce::AudioParameterBool*   lowCutEnabledParam { nullptr };
    juce::AudioParameterFloat*  lowCutFreqParam { nullptr };
    juce::AudioParameterChoice* lowCutSlopeParam { nullptr };
    juce::AudioParameterBool*   highCutEnabledParam { nullptr };
    juce::AudioParameterFloat*  highCutFreqParam { nullptr };
    juce::AudioParameterChoice* highCutSlopeParam { nullptr };

    std::array<PeakBand, Globals::getNumPeakBands()>              peakBands;
    std::array<std::unique_ptr<Node>, Globals::getNumPeakBands()> peakNodes;
    std::array<juce::Point<int>, Globals::getNumPeakBands()>      nodeCoordinates;

    std::array<juce::Value, Globals::getNumPeakBands()> xValues;
    std::array<juce::Value, Globals::getNumPeakBands()> yValues;

    size_t activeNode;
};
