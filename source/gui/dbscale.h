#pragma once

#include <JuceHeader.h>

#include "tick.h"

//==============================================================================
struct DbScale : juce::Component
{
    ~DbScale() override = default;
    void paint(juce::Graphics& g) override;
    void buildBackgroundImage(int dbDivision, juce::Rectangle<int> meterBounds, int minDb, int maxDb);
    static std::vector<Tick> getTicks(int dbDivision, juce::Rectangle<int> meterBounds, int minDb, int maxDb);
private:
    juce::Image bkgd;
};
