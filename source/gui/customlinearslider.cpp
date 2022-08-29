#include "customlinearslider.h"
#include "../utils/colourpalette.h"

//==============================================================================
void CustomLinearSlider::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    g.setColour(ColourPalette::getColour(ColourPalette::Blue));
    g.drawRect(bounds);

    auto paramNormalisableRange = param->getNormalisableRange();
    auto scaledValue = juce::jmap<float>(param->convertFrom0to1(param->getValue()), paramNormalisableRange.start, paramNormalisableRange.end, bounds.getBottom(), bounds.getY());
    getLookAndFeel().drawLinearSlider(g,                                         // graphics
                                      bounds.getX(),                             // x
                                      bounds.getY(),                             // y
                                      bounds.getWidth(),                         // width
                                      bounds.getHeight(),                        // height
                                      scaledValue,                               // slider position
                                      bounds.getBottom(),                        // min slider position
                                      bounds.getY(),                             // max slider position
                                      juce::Slider::SliderStyle::LinearVertical, // slider style
                                      *this);                                    // slider
}
