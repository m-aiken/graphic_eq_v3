#include "cutcontrol.h"
#include "../dsp/eqproperties.h"
#include "../utils/colourpalette.h"

//==============================================================================
CutControl::CutControl(juce::AudioProcessorValueTreeState& apvts, const FilterUtils::ChainPositions& _chainPosition)
: chainPosition(_chainPosition)
{
    const auto& params = EqProperties::getCutParams();

    auto freqParamName = params.at(chainPosition == FilterUtils::ChainPositions::LowCut ? EqProperties::CutControls::LOW_CUT_FREQ : EqProperties::CutControls::HIGH_CUT_FREQ);
    auto freqParam     = apvts.getParameter(freqParamName);
    jassert(freqParam != nullptr);
    freqSlider     = std::make_unique<CustomRotaryControl>(*freqParam, "Hz");
    freqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, freqParamName, *freqSlider);

    auto slopeParamName = params.at(chainPosition == FilterUtils::ChainPositions::LowCut ? EqProperties::CutControls::LOW_CUT_SLOPE : EqProperties::CutControls::HIGH_CUT_SLOPE);
    auto slopeParam     = apvts.getParameter(slopeParamName);
    jassert(slopeParam != nullptr);
    slopeSlider     = std::make_unique<CustomLinearSlider>(*slopeParam);
    slopeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, slopeParamName, *slopeSlider);

    addAndMakeVisible(powerButton);

    addAndMakeVisible(*freqSlider);
    addAndMakeVisible(*slopeSlider);
}

void CutControl::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    g.setFont(Globals::getFont());
    auto textHeight = g.getCurrentFont().getHeight();
    g.setColour(ColourPalette::getColour(ColourPalette::Blue));

    g.drawFittedText(chainPosition == FilterUtils::ChainPositions::LowCut ? "LC" : "HC",
                     bounds.getRight() - (textHeight * 2),
                     textHeight * 0.5,
                     textHeight * 2,
                     textHeight,
                     juce::Justification::centred,
                     1);

    auto sliderQuarter = slopeSlider->getWidth() * 0.25;
    auto sliderX       = slopeSlider->getX();
    auto sliderBottom  = slopeSlider->getBottom();

    g.drawFittedText("dB/Oct",
                     sliderX,
                     slopeSlider->getY() - textHeight - 4,
                     slopeSlider->getWidth(),
                     textHeight,
                     juce::Justification::centred,
                     1);

    for (auto i = 0; i < 4; ++i) {
        g.drawFittedText(juce::String((i+1) * 12),
                         sliderX + (sliderQuarter * i),
                         sliderBottom + 4,
                         sliderQuarter,
                         textHeight,
                         juce::Justification::centred,
                         1);
    }

    // Values
    auto valueRectHeight = bounds.getHeight() * 0.4;
    auto valueRect       = juce::Rectangle<int>(0, bounds.getBottom() - valueRectHeight, bounds.getWidth(), valueRectHeight);

    auto vCX = valueRect.getCentreX();
    auto vY  = valueRect.getY();
    auto vW  = valueRect.getWidth() * 0.5;
    auto vH  = valueRect.getHeight() / 3;

    g.drawFittedText("Hz:", 0, vY + vH , vW, vH, juce::Justification::centred, 1);
    g.drawFittedText(juce::String(freqSlider->getValue()), vCX, vY + vH, vW, vH, juce::Justification::centred, 1);
}

void CutControl::resized()
{
    auto bounds   = getLocalBounds();
    auto diameter = (bounds.getWidth() * 0.55);
    auto padding  = Globals::getFont().getHeight();

    powerButton.setBounds(6, 6, 16, 16);

    freqSlider->setBounds(bounds.getCentreX() - (diameter * 0.5),
                          bounds.getCentreX() - (diameter * 0.5),
                          diameter,
                          diameter);

    slopeSlider->setBounds(padding,
                           freqSlider->getBottom() + (padding * 3),
                           bounds.getWidth() - (padding * 2),
                           padding);
}
