#include "peakcontrol.h"
#include "../../dsp/eqproperties.h"
#include "../../utils/colourpalette.h"

//==============================================================================
PeakControl::PeakControl(juce::AudioProcessorValueTreeState& apvts, const int _bandNum)
: bandNum(_bandNum)
{
    auto freqParam = apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, bandNum));
    jassert(freqParam != nullptr);
    freqSlider = std::make_unique<CustomRotaryControl>(*freqParam, "Hz");

    auto gainParam = apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, bandNum));
    jassert(gainParam != nullptr);
    gainSlider = std::make_unique<CustomRotaryControl>(*gainParam, "dB");

    auto qParam = apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, bandNum));
    jassert(qParam != nullptr);
    qSlider = std::make_unique<CustomRotaryControl>(*qParam, "Q");

    freqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, bandNum), *freqSlider);
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, bandNum), *gainSlider);
    qAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, bandNum), *qSlider);

    powerButton.setToggleState(true, juce::NotificationType::dontSendNotification); // probably temporary - should be saved and loaded??

    powerButton.onClick = [this]() {
        bool toggleState = powerButton.getToggleState();

        freqSlider->setEnabled(toggleState);
        gainSlider->setEnabled(toggleState);
        qSlider->setEnabled(toggleState);

        float dim = 0.2f;
        freqSlider->setAlpha(toggleState ? 1.f : dim);
        gainSlider->setAlpha(toggleState ? 1.f : dim);
        qSlider->setAlpha(toggleState ? 1.f : dim);
    };

    addAndMakeVisible(powerButton);
    addAndMakeVisible(*freqSlider);
    addAndMakeVisible(*gainSlider);
    addAndMakeVisible(*qSlider);
}

void PeakControl::paint(juce::Graphics& g)
{
    g.fillAll(ColourPalette::getColour(ColourPalette::Green).withAlpha(0.1f));

    auto bounds = getLocalBounds();

    g.setFont(Globals::getFont());
    auto textHeight = g.getCurrentFont().getHeight();
    g.setColour(ColourPalette::getColour(ColourPalette::Blue));

    g.drawFittedText("P" + juce::String(bandNum),
                     bounds.getRight() - (textHeight * 2),
                     textHeight * 0.5,
                     textHeight * 2,
                     textHeight,
                     juce::Justification::centred,
                     1);

    // Values
    auto valueRectHeight = bounds.getHeight() * 0.4;
    auto valueRect       = juce::Rectangle<int>(0, bounds.getBottom() - valueRectHeight, bounds.getWidth(), valueRectHeight);

    auto vCX = valueRect.getCentreX();
    auto vY  = valueRect.getY();
    auto vW  = valueRect.getWidth() * 0.5;
    auto vH  = valueRect.getHeight() / 3;

    g.drawFittedText("Hz:", 0, vY, vW, vH, juce::Justification::centred, 1);
    g.drawFittedText(juce::String(freqSlider->getValue()), vCX, vY, vW, vH, juce::Justification::centred, 1);
    g.drawFittedText("dB:", 0, vY + vH , vW, vH, juce::Justification::centred, 1);
    g.drawFittedText(juce::String(gainSlider->getValue()), vCX, vY + vH, vW, vH, juce::Justification::centred, 1);
    g.drawFittedText("Q:" , 0, vY + (vH*2), vW, vH, juce::Justification::centred, 1);
    g.drawFittedText(juce::String(qSlider->getValue()) , vCX, vY + (vH*2), vW, vH, juce::Justification::centred, 1);
}

void PeakControl::resized()
{
    auto bounds         = getLocalBounds();
    auto rotaryBounds   = juce::Rectangle<int>(0, 0, bounds.getWidth(), bounds.getHeight() * 0.5);
    auto padding        = 2;
    auto rotaryDiameter = (rotaryBounds.getWidth() * 0.5) - padding;

    powerButton.setBounds(6, 6, 16, 16);

    freqSlider->setBounds(padding,
                          rotaryDiameter * 0.5,
                          rotaryDiameter,
                          rotaryDiameter);

    gainSlider->setBounds(padding,
                          rotaryBounds.getCentreY() + (rotaryDiameter * 0.5),
                          rotaryDiameter,
                          rotaryDiameter);

    qSlider->setBounds(rotaryBounds.getRight() - rotaryDiameter - padding,
                       rotaryBounds.getCentreY(),
                       rotaryDiameter,
                       rotaryDiameter);
}
