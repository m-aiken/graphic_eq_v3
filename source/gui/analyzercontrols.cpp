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
    decayRateSlider = std::make_unique<CustomRotaryControl>(*decayRateParam, "dB/s", "Decay Rate");

    fftOrderAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, params.at(AnalyzerProperties::ParamNames::Analyzer_Points), *fftOrderSlider);
    decayRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, params.at(AnalyzerProperties::ParamNames::Analyzer_Decay_Rate), *decayRateSlider);

    addAndMakeVisible(*fftOrderSlider);
    addAndMakeVisible(*decayRateSlider);
}

void AnalyzerControls::paint(juce::Graphics& g)
{
    g.fillAll(ColourPalette::getColour(ColourPalette::Green).withAlpha(0.1f));

    auto bounds = getLocalBounds();
    auto padding = 6;
    auto labelHeight = 20;
    auto textHeight = 14;
    /*
    g.setFont(Globals::getFont());
    g.setColour(ColourPalette::getColour(ColourPalette::Blue));

    g.drawFittedText("Analyzer Controls",
                     bounds.getX(),
                     bounds.getY(),
                     bounds.getWidth(),
                     labelHeight,
                     juce::Justification::centred,
                     1);

    g.drawFittedText("Points",
                     bounds.getX(),
                     bounds.getY(),
                     bounds.getWidth() * 0.5,
                     labelHeight,
                     juce::Justification::centredLeft,
                     1);

    g.drawFittedText("8192",
                     fftOrderSlider->getRight() + padding,
                     fftOrderSlider->getY(),
                     60,
                     textHeight,
                     juce::Justification::centredLeft,
                     1);

    g.drawFittedText("4096",
                     fftOrderSlider->getRight() + padding,
                     (fftOrderSlider->getBottom() - fftOrderSlider->getHeight() * 0.5) - (textHeight * 0.5),
                     60,
                     textHeight,
                     juce::Justification::centredLeft,
                     1);

    g.drawFittedText("2048",
                     fftOrderSlider->getRight() + padding,
                     fftOrderSlider->getBottom() - textHeight,
                     60,
                     textHeight,
                     juce::Justification::centredLeft,
                     1);
    */
}

void AnalyzerControls::resized()
{
    /*
    auto bounds = getLocalBounds();
    auto sliderWidth = 16;
    auto padding = 6;
    auto labelHeight = 20;
    auto rotaryDiameter = (bounds.getWidth() * 0.5) - (padding * 2);

    fftOrderSlider->setBounds(padding,
                              labelHeight + padding,
                              sliderWidth,
                              rotaryDiameter);

    decayRateSlider->setBounds(bounds.getCentreX() + padding,
                               labelHeight + padding,
                               rotaryDiameter,
                               rotaryDiameter);
    */
}
