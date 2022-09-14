#include "filterutils.h"

//==============================================================================
void FilterUtils::updatePeakCoefficients(MonoChain& monoChain,
                                         const ChainPositions& chainPosition,
                                         juce::AudioParameterFloat* freqParam,
                                         juce::AudioParameterFloat* qParam,
                                         juce::AudioParameterFloat* gainParam,
                                         double sampleRate)
{
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
                                                                                freqParam->get(),
                                                                                qParam->get(),
                                                                                juce::Decibels::decibelsToGain(gainParam->get()));

    switch (chainPosition) {
        case ChainPositions::Peak_0: {
            *monoChain.get<ChainPositions::Peak_0>().coefficients = *peakCoefficients;
            break;
        }
        case ChainPositions::Peak_1: {
            *monoChain.get<ChainPositions::Peak_1>().coefficients = *peakCoefficients;
            break;
        }
        case ChainPositions::Peak_2: {
            *monoChain.get<ChainPositions::Peak_2>().coefficients = *peakCoefficients;
            break;
        }
        case ChainPositions::Peak_3: {
            *monoChain.get<ChainPositions::Peak_3>().coefficients = *peakCoefficients;
            break;
        }
        case ChainPositions::Peak_4: {
            *monoChain.get<ChainPositions::Peak_4>().coefficients = *peakCoefficients;
            break;
        }
        case ChainPositions::Peak_5: {
            *monoChain.get<ChainPositions::Peak_5>().coefficients = *peakCoefficients;
            break;
        }
    }
}

void FilterUtils::updateCutCoefficients(MonoChain& monoChain,
                                        const ChainPositions& chainPosition,
                                        CoefficientsType& coefficients,
                                        juce::AudioParameterChoice* slopeParam)
{
    auto& cutFilterChain = chainPosition == ChainPositions::LowCut ? monoChain.get<ChainPositions::LowCut>() : monoChain.get<ChainPositions::HighCut>();
    cutFilterChain.setBypassed<Slope_12>(true);
    cutFilterChain.setBypassed<Slope_24>(true);
    cutFilterChain.setBypassed<Slope_36>(true);
    cutFilterChain.setBypassed<Slope_48>(true);

    switch (slopeParam->getIndex()) {
        case Slope_48: {
            *cutFilterChain.template get<Slope_48>().coefficients = *coefficients[Slope_48];
            cutFilterChain.setBypassed<Slope_48>(false);
        }
        case Slope_36: {
            *cutFilterChain.template get<Slope_36>().coefficients = *coefficients[Slope_36];
            cutFilterChain.setBypassed<Slope_36>(false);
        }
        case Slope_24: {
            *cutFilterChain.template get<Slope_24>().coefficients = *coefficients[Slope_24];
            cutFilterChain.setBypassed<Slope_24>(false);
        }
        case Slope_12: {
            *cutFilterChain.template get<Slope_12>().coefficients = *coefficients[Slope_12];
            cutFilterChain.setBypassed<Slope_12>(false);
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
