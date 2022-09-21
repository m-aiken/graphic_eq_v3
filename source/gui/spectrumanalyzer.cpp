#include "spectrumanalyzer.h"
#include "../dsp/analyzerproperties.h"
#include "../utils/colourpalette.h"

//==============================================================================
SpectrumAnalyzer::SpectrumAnalyzer(double _sampleRate,
                                   SingleChannelSampleFifo<juce::AudioBuffer<float>>& leftScsf,
                                   SingleChannelSampleFifo<juce::AudioBuffer<float>>& rightScsf,
                                   juce::AudioProcessorValueTreeState& apvts)
: sampleRate(_sampleRate),
  leftPathProducer(_sampleRate, leftScsf),
  rightPathProducer(_sampleRate, rightScsf)
{
    const auto& params = AnalyzerProperties::getAnalyzerParams();

    auto initListener = [&](auto& listener, const auto& paramName, const auto& lambda) {
        auto param = apvts.getParameter(params.at(paramName));
        jassert(param != nullptr);
        listener = std::make_unique<ParamListener<float>>(*param, lambda);
    };

    initListener(analyzerOrderParamListener,
                 AnalyzerProperties::ParamNames::Analyzer_Points,
                 [this](const auto& newOrder){ updateOrder(newOrder); });

    initListener(analyzerDecayRateParamListener,
                 AnalyzerProperties::ParamNames::Analyzer_Decay_Rate,
                 [this](const auto& decayRate){ updateDecayRate(decayRate); });

    auto orderParam = apvts.getParameter(params.at(AnalyzerProperties::ParamNames::Analyzer_Points));
    updateOrder(orderParam->getValue());

    auto decayRateParam = apvts.getParameter(params.at(AnalyzerProperties::ParamNames::Analyzer_Decay_Rate));
    updateDecayRate(decayRateParam->convertFrom0to1(decayRateParam->getValue()));

    leftPathProducer.toggleProcessing(true);
    rightPathProducer.toggleProcessing(true);

    animate();
}

void SpectrumAnalyzer::timerCallback()
{
    if (!active) {
        leftAnalyzerPath.clear();
        rightAnalyzerPath.clear();
        stopTimer();
    }
    else {
        if ( leftPathProducer.getNumAvailableForReading() > 0 ) {
            while ( leftPathProducer.pull(leftAnalyzerPath) ) { } // get most recent
        }

        if ( rightPathProducer.getNumAvailableForReading() > 0 ) {
            while ( rightPathProducer.pull(rightAnalyzerPath) ) { } // get most recent
        }
    }
    repaint();
}

void SpectrumAnalyzer::resized()
{
    auto bounds = getLocalBounds();
    leftPathProducer.setFFTRectBounds(bounds.toFloat());
    rightPathProducer.setFFTRectBounds(bounds.toFloat());
}

void SpectrumAnalyzer::paint(juce::Graphics& g)
{
    paintBackground(g);
    auto bounds = getLocalBounds();

    g.fillAll(ColourPalette::getColour(ColourPalette::Green).withAlpha(0.1f));

    leftAnalyzerPath.applyTransform(juce::AffineTransform().translation(bounds.getX(), bounds.getY()));
    rightAnalyzerPath.applyTransform(juce::AffineTransform().translation(bounds.getX(), bounds.getY()));


    g.setColour(juce::Colours::blue);
//    g.setColour(ColourPalette::getColour(ColourPalette::Blue));
    g.strokePath(leftAnalyzerPath, juce::PathStrokeType(1.f));

    g.setColour(juce::Colours::green);
//    g.setColour(ColourPalette::getColour(ColourPalette::Green));
    g.strokePath(rightAnalyzerPath, juce::PathStrokeType(1.f));
}

void SpectrumAnalyzer::customizeScales(int lsMin, int lsMax, int rsMin, int rsMax, int division)
{
    leftScaleMin = lsMin;
    leftScaleMax = lsMax;
    rightScaleMin = rsMin;
    rightScaleMax = rsMax;

    leftPathProducer.changePathRange(leftScaleMin, leftScaleMax);
    rightPathProducer.changePathRange(leftScaleMin, leftScaleMax);

    if (!getLocalBounds().isEmpty()) {
        repaint();
    }
}

void SpectrumAnalyzer::paintBackground(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    const float minFreq = Globals::getMinFrequency();
    const float maxFreq = Globals::getMaxFrequency();
    const float minDb = Globals::getNegativeInf();
    const float maxDb = Globals::getMaxDecibels();
    auto boundsX = bounds.getX();
    auto boundsY = bounds.getY();
    auto boundsWidth = bounds.getWidth();
    auto boundsBottom = bounds.getBottom();
    auto boundsRight = bounds.getRight();

    g.setFont(Globals::getFont());

    g.setColour(ColourPalette::getColour(ColourPalette::Blue).withAlpha(0.2f));

    // dB markers
    for ( auto i = minDb; i <= maxDb; i += 12 ) {
        auto yCoord = juce::jmap<float>(i, minDb, maxDb, boundsBottom, boundsY);
        g.drawLine(boundsX + 2, yCoord, boundsRight - 2, yCoord, 1.f);
    }

    // Frequency markers
    /*
    std::vector<float> freqs { 20.f, 50.f, 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f, 10000.f, 20000.f };

    for ( auto i = 1; i < freqs.size() - 1; ++i ) {
        auto normalizedX = juce::mapFromLog10<float>(freqs[i], minFreq, maxFreq);
        auto freqLineX = boundsX + boundsWidth * normalizedX;

        g.setColour(ColourPalette::getColour(ColourPalette::Blue).withAlpha(0.2f));
        g.drawVerticalLine(freqLineX, boundsY, boundsBottom);
    }
    */
    auto inCatchGroup = [](int i) -> bool {
        return (i <= 50 && i % 10 == 0) ||
               (i <= 200 && i % 20 == 0) ||
               (i <= 1000 && i % 100 == 0) ||
               (i <= 2000 && i % 200 == 0) ||
               (i <= 5000 && i % 500 == 0) ||
               (i % 1000 == 0);
    };

    for ( auto i = minFreq + 1; i < maxFreq - 1; ++i ) {
        if (inCatchGroup(i)) {
            auto alpha = i == 50 || i == 100 || i == 200 || i == 500 || i == 1000 || i == 2000 || i == 5000 || i == 10000 ? 0.2f : 0.07f;
            g.setColour(ColourPalette::getColour(ColourPalette::Blue).withAlpha(alpha));
            auto normalizedX = juce::mapFromLog10<float>(i, minFreq, maxFreq);
            auto freqLineX = boundsX + boundsWidth * normalizedX;
            g.drawVerticalLine(freqLineX, boundsY, boundsBottom);
        }
    }
}

void SpectrumAnalyzer::setActive(bool activeState)
{
    active = activeState;
    if (active && !isTimerRunning()) {
        animate();
    }
}

void SpectrumAnalyzer::updateDecayRate(float decayRate)
{
    leftPathProducer.setDecayRate(decayRate);
    rightPathProducer.setDecayRate(decayRate);
}

void SpectrumAnalyzer::updateOrder(float value)
{
    auto denormalizedVal = static_cast<int>(std::floor(juce::jmap<float>(value, 0.f, 1.f, 11.f, 13.f)));
    leftPathProducer.changeOrder(static_cast<Globals::FFTOrder>(denormalizedVal));
    rightPathProducer.changeOrder(static_cast<Globals::FFTOrder>(denormalizedVal));
}

void SpectrumAnalyzer::animate()
{
    startTimerHz(60);
}
