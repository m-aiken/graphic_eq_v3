#pragma once

#include <JuceHeader.h>

#include "../utils/globals.h"

//==============================================================================
namespace EqProperties
{

    enum ParamNames
    {
        LOW_CUT_FREQ,
        LOW_CUT_SLOPE,
        HIGH_CUT_FREQ,
        HIGH_CUT_SLOPE,
        PEAK_FREQ,
        PEAK_GAIN,
        PEAK_Q
    };

    inline const std::map<ParamNames, juce::String>& getEqParams()
    {
        static std::map<ParamNames, juce::String> paramNamesMap = {
                { LOW_CUT_FREQ,   "Low Cut Freq" },
                { LOW_CUT_SLOPE,  "Low Cut Slope" },
                { HIGH_CUT_FREQ,  "High Cut Freq" },
                { HIGH_CUT_SLOPE, "High Cut Slope" },
                { PEAK_FREQ,      "Peak Freq" },
                { PEAK_GAIN,      "Peak Gain" },
                { PEAK_Q,         "Peak Q" },
        };

        return paramNamesMap;
    }

    inline void addEqParams(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
    {
        const auto& params = getEqParams();

        juce::StringArray cutChoices;

        for (auto i = 0; i < 4; ++i)
        {
            juce::String str;
            str << (12 + i*12);
            str << " dB/Oct";
            cutChoices.add(str);
        }

        auto freqNormalisableRange = juce::NormalisableRange(Globals::getMinFrequency(), Globals::getMaxFrequency(), 1.f, 0.25f);
        auto gainNormalisableRange = juce::NormalisableRange(Globals::getNegativeInf(), Globals::getMaxDecibels(), 0.5f, 1.f);

        layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(LOW_CUT_FREQ),
                                                               params.at(LOW_CUT_FREQ),
                                                               freqNormalisableRange,
                                                               20.f));

        layout.add(std::make_unique<juce::AudioParameterChoice>(params.at(LOW_CUT_SLOPE),
                                                                params.at(LOW_CUT_SLOPE),
                                                                cutChoices,
                                                                0));

        layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(HIGH_CUT_FREQ),
                                                               params.at(HIGH_CUT_FREQ),
                                                               freqNormalisableRange,
                                                               20000.f));

        layout.add(std::make_unique<juce::AudioParameterChoice>(params.at(HIGH_CUT_SLOPE),
                                                                params.at(HIGH_CUT_SLOPE),
                                                                cutChoices,
                                                                0));

        layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(PEAK_FREQ),
                                                               params.at(PEAK_FREQ),
                                                               freqNormalisableRange,
                                                               750.f));

        layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(PEAK_GAIN),
                                                               params.at(PEAK_GAIN),
                                                               gainNormalisableRange,
                                                               0.f));

        layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(PEAK_Q),
                                                               params.at(PEAK_Q),
                                                               juce::NormalisableRange(0.1f, 10.f, 0.05f, 1.f),
                                                               1.f));
    }

}
