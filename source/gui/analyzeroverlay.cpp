#include "analyzeroverlay.h"
#include "../utils/colourpalette.h"

//==============================================================================
void AnalyzerOverlay::paint(juce::Graphics& g)
{
    g.reduceClipRegion(fftBoundingBox);
    g.setColour(ColourPalette::getColour(ColourPalette::Salmon).withAlpha(0.1f));
    g.fillRect(fftBoundingBox);
}

void AnalyzerOverlay::resized()
{
    AnalyzerBase::resized();
}

void AnalyzerOverlay::mouseDrag(const juce::MouseEvent& event)
{

}
