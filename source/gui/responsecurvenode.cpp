#include "responsecurvenode.h"
#include "../utils/colourpalette.h"

//==============================================================================
void ResponseCurveNode::paint(juce::Graphics& g)
{
    g.setColour(ColourPalette::getColour(ColourPalette::Salmon));
    g.fillEllipse(getLocalBounds().toFloat());
}

juce::Point<int> ResponseCurveNode::getCoordinates()
{
    return position;
}

void ResponseCurveNode::setCoordinates(int xCoord, int yCoord)
{
    position.setXY(xCoord, yCoord);
}
