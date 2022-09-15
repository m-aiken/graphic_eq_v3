#include "cutcontrol.h"
#include "../dsp/eqproperties.h"
#include "../utils/colourpalette.h"

//==============================================================================
CutControl::CutControl(juce::AudioProcessorValueTreeState& apvts, const FilterUtils::ChainPositions& _chainPosition)
: chainPosition(_chainPosition)
{
    const auto& params = EqProperties::getCutParams();
    auto freqParamName = params.at(chainPosition == FilterUtils::ChainPositions::LowCut ? EqProperties::CutControls::LOW_CUT_FREQ : EqProperties::CutControls::HIGH_CUT_FREQ);
    auto freqParam = apvts.getParameter(freqParamName);
    jassert(freqParam != nullptr);
    freqSlider = std::make_unique<CustomRotaryControl>(*freqParam, "Hz", "Hz");
    freqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, freqParamName, *freqSlider);

    addAndMakeVisible(*freqSlider);
}

void CutControl::paint(juce::Graphics& g)
{
    g.fillAll(ColourPalette::getColour(ColourPalette::Green).withAlpha(0.1f));

    auto bounds = getLocalBounds();

    g.setFont(Globals::getFont().withHeight(12.f));
    auto textHeight = g.getCurrentFont().getHeight();
    g.setColour(ColourPalette::getColour(ColourPalette::Blue));

    g.drawFittedText(chainPosition == FilterUtils::ChainPositions::LowCut ? "LC" : "HC",
                     bounds.getRight() - (textHeight * 2),
                     textHeight * 0.5,
                     textHeight * 2,
                     textHeight,
                     juce::Justification::centred,
                     1);

    // Values
    auto valueRect = juce::Rectangle<int>(0, bounds.getCentreY(), bounds.getWidth(), bounds.getHeight() * 0.5);
    auto vCX = valueRect.getCentreX();
    auto vY = valueRect.getY();
    auto vW = valueRect.getWidth() * 0.5;
    auto vH = valueRect.getHeight() / 3;

    g.drawFittedText("Hz:", 0, vY, vW, vH, juce::Justification::centred, 1);
    g.drawFittedText(juce::String(freqSlider->getValue()), vCX, vY, vW, vH, juce::Justification::centred, 1);
}

void CutControl::resized()
{
    auto bounds = getLocalBounds();
    auto diameter = (bounds.getWidth() * 0.65);

    freqSlider->setBounds(bounds.getCentreX() - (diameter * 0.5),
                          bounds.getCentreX() - (diameter * 0.5),
                          diameter,
                          diameter);
}
