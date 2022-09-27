#include "peakcontrol.h"
#include "../../dsp/eqproperties.h"
#include "../../utils/colourpalette.h"

//==============================================================================
PeakControl::PeakControl(juce::AudioProcessorValueTreeState& apvts, const int _bandNum)
: bandNum(_bandNum)
{
    powerButton      = std::make_unique<PowerButton>();
    buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, EqProperties::getPeakControlParamName(EqProperties::PeakControl::ENABLED, bandNum), *powerButton);

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

    powerButton->onClick = [this]() {
        bool toggleState = powerButton->getToggleState();

        freqSlider->setEnabled(toggleState);
        gainSlider->setEnabled(toggleState);
        qSlider->setEnabled(toggleState);

        float dim = 0.2f;
        freqSlider->setAlpha(toggleState ? 1.f : dim);
        gainSlider->setAlpha(toggleState ? 1.f : dim);
        qSlider->setAlpha(toggleState ? 1.f : dim);
    };

    addAndMakeVisible(*powerButton);
    addAndMakeVisible(*freqSlider);
    addAndMakeVisible(*gainSlider);
    addAndMakeVisible(*qSlider);
}

void PeakControl::paint(juce::Graphics& g)
{
    auto nodeFocusOnColour  = ColourPalette::getColour(ColourPalette::Green).withAlpha(0.1f);
    auto nodeFocusOffColour = ColourPalette::getColour(ColourPalette::Eggshell);
    g.fillAll(nodeIsActive ? nodeFocusOnColour : nodeFocusOffColour);

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
    auto labelRectHeight = bounds.getHeight() * 0.4;
    auto labelRect       = juce::Rectangle<int>(0, bounds.getBottom() - labelRectHeight, bounds.getWidth(), labelRectHeight);

    auto centreX     = labelRect.getCentreX();
    auto labelRectY  = labelRect.getY();
    auto labelWidth  = labelRect.getWidth() * 0.5;
    auto labelHeight = labelRect.getHeight() / 3;
    auto padding     = Globals::getFont().getHeight();

    std::array<std::pair<juce::String, juce::String>, 3> paramValues {
            std::pair{ "Hz:", juce::String(freqSlider->getValue()) },
            std::pair{ "dB:", juce::String(gainSlider->getValue()) },
            std::pair{ "Q:",  juce::String(qSlider->getValue()) }
    };

    for (size_t i = 0; i < paramValues.size(); ++i) {
        // label
        g.drawFittedText(paramValues.at(i).first,
                         0,
                         labelRectY + (labelHeight * i),
                         labelWidth - padding,
                         labelHeight,
                         juce::Justification::centredRight,
                         1);

        // value
        g.drawFittedText(paramValues.at(i).second,
                         centreX + padding,
                         labelRectY + (labelHeight * i),
                         labelWidth - padding,
                         labelHeight,
                         juce::Justification::centredLeft,
                         1);
    }
}

void PeakControl::resized()
{
    auto bounds         = getLocalBounds();
    auto rotaryBounds   = juce::Rectangle<int>(0, 0, bounds.getWidth(), bounds.getHeight() * 0.5);
    auto padding        = 2;
    auto rotaryDiameter = (rotaryBounds.getWidth() * 0.5) - padding;

    powerButton->setBounds(6, 6, 16, 16);

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

void PeakControl::setNodeIsActive(bool activeState)
{
    nodeIsActive = activeState;
    repaint();
}
