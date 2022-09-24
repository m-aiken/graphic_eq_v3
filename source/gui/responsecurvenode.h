#pragma once

#include <JuceHeader.h>

#include "../utils/globals.h"

//==============================================================================
struct ResponseCurveNode : juce::Component
{
    void paint(juce::Graphics& g) override;
    juce::Point<int> getCoordinates();
    void setCoordinates(int xCoord, int yCoord);
private:
    juce::Point<int> position { 0,0 };
};

//==============================================================================
struct ResponseCurveNodes : juce::Component
{
    ResponseCurveNodes();
    void resized() override;
    std::array<juce::Point<int>, Globals::getNumPeakBands()> getCoordinates();
    void setCoordinates(size_t nodeIndex, int xCoord, int yCoord);
private:
    int nodeDiameter { 12 };
    int nodeRadius { 6 };
    std::array<ResponseCurveNode, Globals::getNumPeakBands()> nodes;
    std::array<juce::Point<int>, Globals::getNumPeakBands()> nodeCoordinates;
};
