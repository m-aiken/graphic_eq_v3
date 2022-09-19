#include "powerbutton.h"
#include "../utils/colourpalette.h"

//==============================================================================
void PowerButton::paint(juce::Graphics& g)
{
    g.fillAll(ColourPalette::getColour(ColourPalette::Eggshell));

    auto bounds = getLocalBounds();
    float lineThickness = 2.f;

    g.setColour(ColourPalette::getColour(ColourPalette::Blue));
    g.drawEllipse(bounds.toFloat().reduced(lineThickness), lineThickness);

    g.setColour(ColourPalette::getColour(ColourPalette::Eggshell));
    g.fillRect(bounds.getCentreX() - lineThickness, 0.f, lineThickness * 2, bounds.getHeight() * 0.5f);

    g.setColour(ColourPalette::getColour(ColourPalette::Blue));
    g.fillRect(bounds.getCentreX() - (lineThickness * 0.5), lineThickness * 0.5, lineThickness, bounds.getHeight() * 0.4f);
}
