#include "analyzeroverlay.h"
#include "../utils/colourpalette.h"
#include "../utils/globals.h"

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
    auto boundsWidth = fftBoundingBox.getWidth();
    auto boundsX = fftBoundingBox.getX();
    auto boundsY = fftBoundingBox.getY();
    auto boundsRight = fftBoundingBox.getRight();
    auto responseCurveMin = fftBoundingBox.getBottom();
    auto responseCurveMax = fftBoundingBox.getY();

    auto xCoord = event.position.getX() - boundsX;
    auto yCoord = event.position.getY() - boundsY;

    auto xFrequency = mapToLog10<double>(static_cast<double>(xCoord) / boundsWidth,
                                         Globals::getMinFrequency(),
                                         Globals::getMaxFrequency());

    // map y coord to magnitude, then gert ??? (the value to update the gain param) from magnitude

    if (xFrequency >= Globals::getMinFrequency() && xFrequency <= Globals::getMaxFrequency()) {
        // update frequency params
    }

    DBG(juce::String("x: ") << xFrequency);
    DBG(juce::String("y: ") << yCoord);
}
