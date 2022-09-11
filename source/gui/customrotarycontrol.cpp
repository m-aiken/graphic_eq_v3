#include "customrotarycontrol.h"
#include "../utils/colourpalette.h"
#include "../utils/globals.h"

//==============================================================================
CustomRotaryControl::CustomRotaryControl(juce::RangedAudioParameter& rap, const juce::String& unitSuffix, const juce::String& title)
: juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox),
  param(&rap),
  suffix(unitSuffix)
{
    setName(title);
}

void CustomRotaryControl::paint(juce::Graphics& g)
{
    auto startAngle = juce::degreesToRadians(180.f + 45.f);
    auto endAngle = juce::degreesToRadians(180.f - 45.f) + juce::MathConstants<float>::twoPi;

    auto range = param->getNormalisableRange();
    auto bounds = getLocalBounds();

    g.setFont(Globals::getFont());
    g.setColour(ColourPalette::getColour(ColourPalette::Blue));

    g.drawFittedText(getName(),
                     bounds.removeFromTop(getTextHeight()),
                     juce::Justification::centred,
                     1);

    auto valueToDraw = juce::jmap<float>(getValue(),
                                         range.start,
                                         range.end,
                                         startAngle,
                                         endAngle);

    auto rotaryBounds = getRotaryBounds().reduced(4);

    getLookAndFeel().drawRotarySlider(g,
                                      rotaryBounds.getX(),      // x
                                      rotaryBounds.getY(),      // y
                                      rotaryBounds.getWidth(),  // width
                                      rotaryBounds.getHeight(), // height
                                      valueToDraw,              // position
                                      startAngle,               // start angle
                                      endAngle,                 // end angle
                                      *this);                   // slider

    auto center = rotaryBounds.toFloat().getCentre();
    auto radius = rotaryBounds.getWidth() * 0.5f;

    g.setFont(rotaryBounds.getHeight() * 0.2);

    // current value string
    g.setColour(ColourPalette::getColour(ColourPalette::Blue));
    juce::Rectangle<float> r;
    auto str = getDisplayString();
    r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight());
    r.setCentre(center);
    g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
}

juce::Rectangle<int> CustomRotaryControl::getRotaryBounds() const
{
    auto bounds = getLocalBounds();

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    size -= getTextHeight() * 2;

    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(bounds.getCentreY() - (size * 0.5));

    return r;
}

juce::String CustomRotaryControl::getDisplayString() const
{
    juce::String str;

    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param)) {
        auto currentChoice = choiceParam->getCurrentChoiceName();
        str = ( currentChoice.substring(currentChoice.indexOf(".")) == ".5" )
              ? currentChoice
              : currentChoice.substring(0, currentChoice.indexOf("."));
    }
    else if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param)) {
        str = juce::String(getValue());
    }
    else {
        jassertfalse;
    }

    str << suffix;

    return str;
}
