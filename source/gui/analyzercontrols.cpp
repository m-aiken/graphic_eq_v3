#include "analyzercontrols.h"
#include "../dsp/analyzerproperties.h"
#include "../utils/colourpalette.h"
#include "../utils/globals.h"

//==============================================================================
AnalyzerControls::AnalyzerControls(juce::AudioProcessorValueTreeState& apvts)
{
    const auto& params = AnalyzerProperties::getAnalyzerParams();

    auto fftOrderParam = apvts.getParameter(params.at(AnalyzerProperties::ParamNames::Analyzer_Points));
    jassert(fftOrderParam != nullptr);
    fftOrderSlider = std::make_unique<CustomLinearSlider>(*fftOrderParam);

    auto decayRateParam = apvts.getParameter(params.at(AnalyzerProperties::ParamNames::Analyzer_Decay_Rate));
    jassert(decayRateParam != nullptr);
    decayRateSlider = std::make_unique<CustomLinearSlider>(*decayRateParam);

    fftOrderAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, params.at(AnalyzerProperties::ParamNames::Analyzer_Points), *fftOrderSlider);
    decayRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, params.at(AnalyzerProperties::ParamNames::Analyzer_Decay_Rate), *decayRateSlider);

    addAndMakeVisible(*fftOrderSlider);
    addAndMakeVisible(*decayRateSlider);
}

void AnalyzerControls::paint(juce::Graphics& g)
{
    auto bounds       = getLocalBounds();
    auto sliderWidth  = bounds.getWidth() * 0.4;
    auto sliderHeight = Globals::getFont().getHeight();

    g.setFont(Globals::getFont());
    g.setColour(ColourPalette::getColourV2(ColourPalette::Text));

    g.drawFittedText("FFT Order (2048/4096/8192)",
                     0,
                     0,
                     sliderWidth,
                     sliderHeight,
                     juce::Justification::centred,
                     1);

    g.drawFittedText("Analyzer Decay Rate (dB/s)",
                     bounds.getRight() - sliderWidth,
                     0,
                     sliderWidth,
                     sliderHeight,
                     juce::Justification::centred,
                     1);
}

void AnalyzerControls::resized()
{
    auto bounds       = getLocalBounds();
    auto sliderWidth  = bounds.getWidth() * 0.4;
    auto sliderHeight = Globals::getFont().getHeight();
    auto padding      = sliderHeight * 0.2;

    fftOrderSlider->setBounds(0,
                              sliderHeight + padding,
                              sliderWidth,
                              sliderHeight);

    decayRateSlider->setBounds(bounds.getRight() - sliderWidth,
                               sliderHeight + padding,
                               sliderWidth,
                               sliderHeight);
}
