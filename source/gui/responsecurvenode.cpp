#include "responsecurvenode.h"

//==============================================================================
ResponseCurveNodes::ResponseCurveNodes()
{
    for (auto& node : nodes) {
        addAndMakeVisible(node);
    }

    for (auto& coord : nodeCoordinates) {
        coord.setXY(0, 0);
    }
}

void ResponseCurveNodes::resized()
{
    auto& coords = getCoordinates();
    for (size_t i = 0; i < nodes.size(); ++i) {
        nodes.at(i).setBounds(coords.at(i).getX() - nodeRadius, coords.at(i).getY() - nodeRadius, nodeDiameter, nodeDiameter);
    }
}

std::array<juce::Point<int>, Globals::getNumPeakBands()>& ResponseCurveNodes::getCoordinates()
{
    return nodeCoordinates;
}

void ResponseCurveNodes::setCoordinates(size_t nodeIndex, int xCoord, int yCoord)
{
    nodeCoordinates.at(nodeIndex).setXY(xCoord, yCoord);
    resized();
}
