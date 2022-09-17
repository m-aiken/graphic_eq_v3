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
    auto bounds = getLocalBounds();

    g.setColour(ColourPalette::getColour(ColourPalette::Green).withAlpha(0.1f));

    auto lineThickness = 4.f;
    auto offset = lineThickness * 0.5f;
    auto lineTop = bounds.getY();
    auto lineBottom = bounds.getBottom();
    auto boundsDivision = bounds.getWidth() * 0.25;
    // startX, startY, endX, endY, lineThickness
    /*
    g.drawLine(bounds.getX(),
               lineTop,
               bounds.getX(),
               lineBottom,
               lineThickness);
    */
    /*
    g.drawLine(boundsDivision - offset,
               lineTop,
               boundsDivision - offset,
               lineBottom,
               lineThickness);

    g.drawLine((boundsDivision * 2) - offset,
               lineTop,
               (boundsDivision * 2) - offset,
               lineBottom,
               lineThickness);

    g.drawLine((boundsDivision * 3) - offset,
               lineTop,
               (boundsDivision * 3) - offset,
               lineBottom,
               lineThickness);
    */
    /*
    g.drawLine(bounds.getRight() - lineThickness,
               lineTop,
               bounds.getRight() - lineThickness,
               lineBottom,
               lineThickness);
    */

    auto sliderHeight = bounds.getHeight() / 3;
    auto sliderWidth = bounds.getWidth() * 0.2;

    g.setFont(Globals::getFont().withHeight(sliderHeight));
    g.setColour(ColourPalette::getColour(ColourPalette::Blue));

    g.drawFittedText("FFT Order (2048/4096/8192)",
                     bounds.getCentreX(),
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
    auto bounds = getLocalBounds();
    auto sliderWidth = bounds.getWidth() * 0.2;
    auto sliderHeight = bounds.getHeight() / 3;
    auto padding = 6;

    fftOrderSlider->setBounds(bounds.getCentreX(),
                              sliderHeight,
                              sliderWidth,
                              sliderHeight);

    decayRateSlider->setBounds(bounds.getRight() - sliderWidth,
                               sliderHeight,
                               sliderWidth,
                               sliderHeight);
}
