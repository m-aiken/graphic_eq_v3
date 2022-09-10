#pragma once

#include <JuceHeader.h>

//==============================================================================
struct ResponseCurveNode : juce::Component
{
    void paint(juce::Graphics& g) override;
    juce::Point<int> getCoordinates();
    void setCoordinates(int xCoord, int yCoord);
private:
    juce::Point<int> position { 0,0 };
};
