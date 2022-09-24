#include "analyzercontainer.h"
#include "../../utils/colourpalette.h"
#include "../../utils/globals.h"

//==============================================================================
AnalyzerContainer::AnalyzerContainer(juce::AudioProcessorValueTreeState& apvts,
                                     double sampleRate,
                                     SingleChannelSampleFifo<juce::AudioBuffer<float>>& leftScsf,
                                     SingleChannelSampleFifo<juce::AudioBuffer<float>>& rightScsf)
: spectrumAnalyzer(sampleRate, leftScsf, rightScsf, apvts),
  responseCurve(apvts, sampleRate)
{
    addAndMakeVisible(backgroundGrid);
    addAndMakeVisible(spectrumAnalyzer);
    addAndMakeVisible(responseCurve);
}

void AnalyzerContainer::paint(juce::Graphics& g)
{
    g.fillAll(ColourPalette::getColour(ColourPalette::Salmon).withAlpha(0.1f));
}

void AnalyzerContainer::resized()
{
    auto bounds = getLocalBounds();
    backgroundGrid.setBounds(bounds);
    spectrumAnalyzer.setBounds(bounds);
    responseCurve.setBounds(bounds);
}

void AnalyzerContainer::mouseDrag(const juce::MouseEvent& event)
{
    auto xCoord = event.position.getX();
    auto yCoord = event.position.getY();

    auto bounds       = getLocalBounds();
    auto boundsWidth  = bounds.getWidth();
    auto boundsHeight = bounds.getHeight();

    if (xCoord >= 0 && xCoord <= boundsWidth && yCoord >= 0 && yCoord <= boundsHeight) {
        auto xFrequency = mapToLog10<double>(static_cast<double>(xCoord) / boundsWidth,
                                             Globals::getMinFrequency(),
                                             Globals::getMaxFrequency());

        auto yDecibels = juce::jmap<float>(yCoord,
                                           0,
                                           boundsHeight,
                                           Globals::getMaxDecibels(),
                                           Globals::getNegativeInf());

        DBG(juce::String("x: ") << xFrequency);
        DBG(juce::String("y: ") << yDecibels);
    }
}
