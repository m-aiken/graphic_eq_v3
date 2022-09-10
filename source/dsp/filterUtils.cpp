#include "filterUtils.h"

//==============================================================================
void FilterUtils::updatePeakCoefficients(MonoChain& monoChain, juce::AudioParameterFloat* freqParam, juce::AudioParameterFloat* qParam, juce::AudioParameterFloat* gainParam, double sampleRate)
{
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
                                                                                freqParam->get(),
                                                                                qParam->get(),
                                                                                juce::Decibels::decibelsToGain(gainParam->get()));

    monoChain.get<Globals::ChainPositions::Peak>().coefficients = peakCoefficients;
}

void FilterUtils::updateCutCoefficients(MonoChain& monoChain, const Globals::ChainPositions& chainPosition, CoefficientsType& coefficients, juce::AudioParameterChoice* slopeParam)
{
    auto& cutFilterChain = chainPosition == Globals::ChainPositions::LowCut ? monoChain.get<Globals::ChainPositions::LowCut>() : monoChain.get<Globals::ChainPositions::HighCut>();
    cutFilterChain.setBypassed<Globals::Slope_12>(true);
    cutFilterChain.setBypassed<Globals::Slope_24>(true);
    cutFilterChain.setBypassed<Globals::Slope_36>(true);
    cutFilterChain.setBypassed<Globals::Slope_48>(true);

    switch (slopeParam->getIndex()) {
        case Globals::Slope_48: {
            cutFilterChain.template get<Globals::Slope_48>().coefficients = coefficients[Globals::Slope_48];
            cutFilterChain.setBypassed<Globals::Slope_48>(false);
        }
        case Globals::Slope_36: {
            cutFilterChain.template get<Globals::Slope_36>().coefficients = coefficients[Globals::Slope_36];
            cutFilterChain.setBypassed<Globals::Slope_36>(false);
        }
        case Globals::Slope_24: {
            cutFilterChain.template get<Globals::Slope_24>().coefficients = coefficients[Globals::Slope_24];
            cutFilterChain.setBypassed<Globals::Slope_24>(false);
        }
        case Globals::Slope_12: {
            cutFilterChain.template get<Globals::Slope_12>().coefficients = coefficients[Globals::Slope_12];
            cutFilterChain.setBypassed<Globals::Slope_12>(false);
            break;
        }
        default: {
            break;
        }
    }
}

FilterUtils::CoefficientsType FilterUtils::makeHighPassFilter(juce::AudioParameterFloat* freqParam,
                                                              juce::AudioParameterChoice* slopeParam,
                                                              double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(freqParam->get(), sampleRate, (slopeParam->getIndex() + 1) * 2);
}

FilterUtils::CoefficientsType FilterUtils::makeLowPassFilter(juce::AudioParameterFloat* freqParam,
                                                             juce::AudioParameterChoice* slopeParam,
                                                             double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(freqParam->get(), sampleRate, (slopeParam->getIndex() + 1) * 2);
}
