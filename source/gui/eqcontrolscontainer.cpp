#include "eqcontrolscontainer.h"

//==============================================================================
EqControlsContainer::EqControlsContainer(juce::AudioProcessorValueTreeState& apvts)
: lowCutControl(apvts, FilterUtils::ChainPositions::LowCut),
  highCutControl(apvts, FilterUtils::ChainPositions::HighCut),
  peakControl0(apvts, 0),
  peakControl1(apvts, 1),
  peakControl2(apvts, 2),
  peakControl3(apvts, 3),
  peakControl4(apvts, 4),
  peakControl5(apvts, 5)
{
    addAndMakeVisible(lowCutControl);
    addAndMakeVisible(highCutControl);

    addAndMakeVisible(peakControl0);
    addAndMakeVisible(peakControl1);
    addAndMakeVisible(peakControl2);
    addAndMakeVisible(peakControl3);
    addAndMakeVisible(peakControl4);
    addAndMakeVisible(peakControl5);
}

void EqControlsContainer::resized()
{
    juce::Grid grid;

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.templateColumns = {
            Track(Fr(1)),
            Track(Fr(1)),
            Track(Fr(1)),
            Track(Fr(1)),
            Track(Fr(1)),
            Track(Fr(1)),
            Track(Fr(1)),
            Track(Fr(1))
    };

    grid.autoRows = Track(Fr(1));

    grid.items.add(juce::GridItem(lowCutControl));
    grid.items.add(juce::GridItem(peakControl0));
    grid.items.add(juce::GridItem(peakControl1));
    grid.items.add(juce::GridItem(peakControl2));
    grid.items.add(juce::GridItem(peakControl3));
    grid.items.add(juce::GridItem(peakControl4));
    grid.items.add(juce::GridItem(peakControl5));
    grid.items.add(juce::GridItem(highCutControl));

    grid.setGap(juce::Grid::Px{4});

    grid.performLayout(getLocalBounds());
}
