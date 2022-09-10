#pragma once

#include <JuceHeader.h>
#include "../utils/globals.h"

//==============================================================================
struct FilterUtils
{
    using Filter = juce::dsp::IIR::Filter<float>;
    // Cut Filter chain: x4 Filters to allow for 12/24/36/48 dB/oct options
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
    using CoefficientsType = juce::ReferenceCountedArray<juce::dsp::FilterDesign<float>::IIRCoefficients>;

    static void updatePeakCoefficients(MonoChain& monoChain, juce::AudioParameterFloat* freqParam, juce::AudioParameterFloat* qParam, juce::AudioParameterFloat* gainParam, double sampleRate);

    static void updateCutCoefficients(MonoChain& monoChain, const Globals::ChainPositions& chainPosition, CoefficientsType& coefficients, juce::AudioParameterChoice* slopeParam);

    static CoefficientsType makeHighPassFilter(juce::AudioParameterFloat* freqParam,
                                               juce::AudioParameterChoice* slopeParam,
                                               double sampleRate);

    static CoefficientsType makeLowPassFilter(juce::AudioParameterFloat* freqParam,
                                              juce::AudioParameterChoice* slopeParam,
                                              double sampleRate);
};
