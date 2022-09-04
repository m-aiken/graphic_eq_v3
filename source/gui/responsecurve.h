#pragma once

#include <JuceHeader.h>
#include "paramlistener.h"

#include "../PluginProcessor.h"

//==============================================================================
struct ResponseCurve : juce::Component
{
    ResponseCurve(juce::AudioProcessorValueTreeState& apvts, double _sampleRate);
    void paint(juce::Graphics& g) override;

private:
//    std::unique_ptr<ParamListener<float>>
    GraphicEqProcessor::MonoChain monoChain;
    double sampleRate;
};
