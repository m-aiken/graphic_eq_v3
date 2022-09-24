#pragma once

#include "JuceHeader.h"
#include "../../utils/globals.h"
#include "../../utils/colourpalette.h"

//==============================================================================
struct Node : juce::Component
{
    void paint(juce::Graphics& g) override
    {
        g.setColour(ColourPalette::getColour(ColourPalette::Salmon));
        g.fillEllipse(getLocalBounds().toFloat());
    }
};

//==============================================================================
struct ResponseCurveNodes : juce::Component
{
    ResponseCurveNodes();
    void resized() override;
    std::array<juce::Point<int>, Globals::getNumPeakBands()>& getCoordinates();
    void setCoordinates(size_t nodeIndex, int xCoord, int yCoord);

private:
    int nodeDiameter { 12 };
    int nodeRadius { 6 };

    std::array<Node, Globals::getNumPeakBands()>             nodes;
    std::array<juce::Point<int>, Globals::getNumPeakBands()> nodeCoordinates;
};
