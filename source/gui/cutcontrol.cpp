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
    freqSlider = std::make_unique<CustomRotaryControl>(*freqParam, "Hz", "Freq");
    freqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, freqParamName, *freqSlider);

    addAndMakeVisible(*freqSlider);
}

void CutControl::paint(juce::Graphics& g)
{
    g.fillAll(ColourPalette::getColour(ColourPalette::Green).withAlpha(0.1f));

    auto bounds = getLocalBounds();
    auto textHeight = 14;

    g.setFont(Globals::getFont());
    g.setColour(ColourPalette::getColour(ColourPalette::Blue));

    g.drawFittedText(chainPosition == FilterUtils::ChainPositions::LowCut ? "LC" : "HC",
                     bounds.getRight() - (textHeight * 2),
                     textHeight * 0.5,
                     textHeight * 2,
                     textHeight,
                     juce::Justification::centred,
                     1);
}

void CutControl::resized()
{
    auto bounds = getLocalBounds();
    auto padding = 2;
    auto diameter = (bounds.getHeight() * 0.5) - (padding * 2);

    freqSlider->setBounds(0,
                          padding,
                          diameter,
                          diameter);
}
