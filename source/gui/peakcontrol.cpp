#include "peakcontrol.h"
#include "../dsp/eqproperties.h"
#include "../utils/colourpalette.h"

//==============================================================================
PeakControl::PeakControl(juce::AudioProcessorValueTreeState& apvts, const int _bandNum)
: bandNum(_bandNum)
{
    auto freqParam = apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, bandNum));
    jassert(freqParam != nullptr);
    freqSlider = std::make_unique<CustomRotaryControl>(*freqParam, "Hz", "F");

    auto gainParam = apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, bandNum));
    jassert(gainParam != nullptr);
    gainSlider = std::make_unique<CustomRotaryControl>(*gainParam, "dB", "G");

    auto qParam = apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, bandNum));
    jassert(qParam != nullptr);
    qSlider = std::make_unique<CustomRotaryControl>(*qParam, "", "Q");

    freqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, bandNum), *freqSlider);
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, bandNum), *gainSlider);
    qAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, bandNum), *qSlider);

    addAndMakeVisible(*freqSlider);
    addAndMakeVisible(*gainSlider);
    addAndMakeVisible(*qSlider);
}

void PeakControl::paint(juce::Graphics& g)
{
    g.fillAll(ColourPalette::getColour(ColourPalette::Green).withAlpha(0.1f));

    auto bounds = getLocalBounds();

    g.setFont(Globals::getFont().withHeight(12.f));
    auto textHeight = g.getCurrentFont().getHeight();
    g.setColour(ColourPalette::getColour(ColourPalette::Blue));

    g.drawFittedText("P" + juce::String(bandNum),
                     bounds.getRight() - (textHeight * 2),
                     textHeight * 0.5,
                     textHeight * 2,
                     textHeight,
                     juce::Justification::centred,
                     1);
}

void PeakControl::resized()
{
    auto bounds = getLocalBounds();
    auto padding = 2;
    auto rotaryDiameter = (bounds.getWidth() * 0.5) - padding;

    freqSlider->setBounds(padding,
                          padding,
                          rotaryDiameter,
                          rotaryDiameter);

    gainSlider->setBounds(padding,
                          bounds.getCentreY() + padding,
                          rotaryDiameter,
                          rotaryDiameter);

    qSlider->setBounds(bounds.getRight() - rotaryDiameter - padding,
                       bounds.getCentreY() - (rotaryDiameter * 0.5),
                       rotaryDiameter,
                       rotaryDiameter);
}
