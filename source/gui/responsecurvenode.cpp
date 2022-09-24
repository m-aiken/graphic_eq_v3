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

//==============================================================================
ResponseCurveNodes::ResponseCurveNodes()
{
    for (auto& node : nodes) {
        addAndMakeVisible(node);
    }
}

void ResponseCurveNodes::resized()
{
    for (auto& node : nodes) {
        auto coordinates = node.getCoordinates();
        node.setBounds(coordinates.getX() - nodeRadius, coordinates.getY() - nodeRadius, nodeDiameter, nodeDiameter);
    }
}

std::array<juce::Point<int>, Globals::getNumPeakBands()> ResponseCurveNodes::getCoordinates()
{
    return nodeCoordinates;
}

void ResponseCurveNodes::setCoordinates(size_t nodeIndex, int xCoord, int yCoord)
{
    nodes.at(nodeIndex).setCoordinates(xCoord, yCoord);
    resized();
}
