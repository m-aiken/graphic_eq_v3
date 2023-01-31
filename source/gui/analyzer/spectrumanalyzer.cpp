#include "spectrumanalyzer.h"
#include "../../dsp/analyzerproperties.h"
#include "../../utils/colourpalette.h"

//==============================================================================
SpectrumAnalyzer::SpectrumAnalyzer(double                                    _sampleRate,
                                   MonoBufferFifo<juce::AudioBuffer<float>>& leftScsf,
                                   MonoBufferFifo<juce::AudioBuffer<float>>& rightScsf,
                                   juce::AudioProcessorValueTreeState&       apvts,
                                   const juce::Colour&                       fillColour)
    : sampleRate(_sampleRate)
    , leftPathProducer(_sampleRate, leftScsf)
    , rightPathProducer(_sampleRate, rightScsf)
    , colour(fillColour)
{
    const auto& params = AnalyzerProperties::getAnalyzerParams();

    auto initListener = [&](auto& listener, const auto& paramName, const auto& lambda) {
        auto param = apvts.getParameter(params.at(paramName));
        jassert(param != nullptr);
        listener = std::make_unique<ParamListener<float>>(*param, lambda);
    };

    initListener(analyzerOrderParamListener,
                 AnalyzerProperties::ParamNames::Analyzer_Points,
                 [this](const auto& newOrder) { updateOrder(newOrder); });

    initListener(analyzerDecayRateParamListener,
                 AnalyzerProperties::ParamNames::Analyzer_Decay_Rate,
                 [this](const auto& decayRate) { updateDecayRate(decayRate); });

    auto orderParam = apvts.getParameter(params.at(AnalyzerProperties::ParamNames::Analyzer_Points));
    updateOrder(orderParam->getValue());

    auto decayRateParam = apvts.getParameter(params.at(AnalyzerProperties::ParamNames::Analyzer_Decay_Rate));
    updateDecayRate(decayRateParam->convertFrom0to1(decayRateParam->getValue()));

    leftPathProducer.toggleProcessing(true);
    rightPathProducer.toggleProcessing(true);

    animate();
}

void SpectrumAnalyzer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    g.fillAll(ColourPalette::getColour(ColourPalette::Green).withAlpha(0.1f));

    leftAnalyzerPath.applyTransform(juce::AffineTransform().translation(bounds.getX(), bounds.getY()));
    rightAnalyzerPath.applyTransform(juce::AffineTransform().translation(bounds.getX(), bounds.getY()));

    g.setColour(colour);
    g.fillPath(leftAnalyzerPath);
    g.fillPath(rightAnalyzerPath);
}

void SpectrumAnalyzer::resized()
{
    auto bounds = getLocalBounds();
    leftPathProducer.setFFTRectBounds(bounds.toFloat());
    rightPathProducer.setFFTRectBounds(bounds.toFloat());
}

void SpectrumAnalyzer::timerCallback()
{
    if (!active) {
        leftAnalyzerPath.clear();
        rightAnalyzerPath.clear();
        stopTimer();
    } else {
        if (leftPathProducer.getNumAvailableForReading() > 0) {
            while (leftPathProducer.pull(leftAnalyzerPath)) {
            } // get most recent
        }

        if (rightPathProducer.getNumAvailableForReading() > 0) {
            while (rightPathProducer.pull(rightAnalyzerPath)) {
            } // get most recent
        }
    }
    repaint();
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
