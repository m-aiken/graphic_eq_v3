#pragma once

#include <JuceHeader.h>

#include "../utils/globals.h"

//==============================================================================
namespace EqProperties
{

    enum CutControls
    {
        LOW_CUT_FREQ,
        LOW_CUT_SLOPE,
        HIGH_CUT_FREQ,
        HIGH_CUT_SLOPE
    };

    enum class PeakControl
    {
        FREQUENCY,
        GAIN,
        QUALITY
    };

    static std::map<PeakControl, juce::String> peakControlMap = {
            { PeakControl::FREQUENCY, "Freq" },
            { PeakControl::GAIN,      "Gain"},
            { PeakControl::QUALITY,   "Q"}
    };

    inline const juce::String getPeakControlParamName(PeakControl peakControl, const int bandNum)
    {
        juce::String str;
        str << "P" << bandNum << " " << peakControlMap.at(peakControl);
        return str;
    }

    inline const std::map<CutControls, juce::String>& getCutParams()
    {
        static std::map<CutControls, juce::String> paramNamesMap = {
                { LOW_CUT_FREQ,   "Low Cut Freq" },
                { LOW_CUT_SLOPE,  "Low Cut Slope" },
                { HIGH_CUT_FREQ,  "High Cut Freq" },
                { HIGH_CUT_SLOPE, "High Cut Slope" }
        };

        return paramNamesMap;
    }

    inline void addCutParams(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
    {
        const auto& params = getCutParams();

        juce::StringArray cutChoices;

        for (auto i = 0; i < 4; ++i)
        {
            juce::String str;
            str << (12 + i*12);
            str << " dB/Oct";
            cutChoices.add(str);
        }

        auto freqNormalisableRange = juce::NormalisableRange(Globals::getMinFrequency(), Globals::getMaxFrequency(), 1.f, 0.25f);

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
    }

    inline void addPeakParams(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
    {
        const auto& params = getCutParams();

        auto freqNormalisableRange = juce::NormalisableRange(Globals::getMinFrequency(), Globals::getMaxFrequency(), 1.f, 0.25f);
        auto gainNormalisableRange = juce::NormalisableRange(Globals::getNegativeInf(), Globals::getMaxDecibels(), 0.5f, 1.f);

        std::vector<float> defaultFreqs { 750.f, 2000.f, 10000.f };

        for (int i = 0; i < Globals::getNumPeakBands(); ++i) {
            layout.add(std::make_unique<juce::AudioParameterFloat>(getPeakControlParamName(PeakControl::FREQUENCY, i),
                                                                   getPeakControlParamName(PeakControl::FREQUENCY, i),
                                                                   freqNormalisableRange,
                                                                   defaultFreqs.at(i)));

            layout.add(std::make_unique<juce::AudioParameterFloat>(getPeakControlParamName(PeakControl::GAIN, i),
                                                                   getPeakControlParamName(PeakControl::GAIN, i),
                                                                   gainNormalisableRange,
                                                                   0.f));

            layout.add(std::make_unique<juce::AudioParameterFloat>(getPeakControlParamName(PeakControl::QUALITY, i),
                                                                   getPeakControlParamName(PeakControl::QUALITY, i),
                                                                   juce::NormalisableRange(0.1f, 10.f, 0.05f, 1.f),
                                                                   1.f));
        }
    }

}
