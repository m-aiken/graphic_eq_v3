#include "responsecurve.h"
#include "../utils/globals.h"
#include "../dsp/eqproperties.h"

//==============================================================================
ResponseCurve::ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate)
: apvts(_apvts),
  sampleRate(_sampleRate)
{
    auto assignFloatParam = [&](auto& target, auto& paramName){
        auto param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(paramName));
        jassert(param != nullptr);
        target = param;
    };

    auto assignChoiceParam = [&](auto& target, auto& paramName){
        auto param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(paramName));
        jassert(param != nullptr);
        target = param;
    };

    const auto& eqParams = EqProperties::getEqParams();
    assignFloatParam(lowCutFreqParam, eqParams.at(EqProperties::ParamNames::LOW_CUT_FREQ));
    assignChoiceParam(lowCutSlopeParam, eqParams.at(EqProperties::ParamNames::LOW_CUT_SLOPE));
    assignFloatParam(highCutFreqParam, eqParams.at(EqProperties::ParamNames::HIGH_CUT_FREQ));
    assignChoiceParam(highCutSlopeParam, eqParams.at(EqProperties::ParamNames::HIGH_CUT_SLOPE));
    assignFloatParam(peakFreqParam, eqParams.at(EqProperties::ParamNames::PEAK_FREQ));
    assignFloatParam(peakGainParam, eqParams.at(EqProperties::ParamNames::PEAK_GAIN));
    assignFloatParam(peakQParam, eqParams.at(EqProperties::ParamNames::PEAK_Q));

    addAndMakeVisible(peakNode);

    addListeners();
    updateMonoChain();
    updateNodeCoordinates(peakNode, peakFreqParam, peakGainParam);
    startTimerHz(60);
}

ResponseCurve::~ResponseCurve()
{
    stopTimer();
    removeListeners();
}

void ResponseCurve::paint(juce::Graphics& g)
{
    g.reduceClipRegion(fftBoundingBox);
    auto boundsWidth = fftBoundingBox.getWidth();
    auto boundsX = fftBoundingBox.getX();
    auto responseCurveMin = fftBoundingBox.getBottom();
    auto responseCurveMax = fftBoundingBox.getY();

    auto& lowCut = monoChain.get<Globals::ChainPositions::LowCut>();
    auto& peak = monoChain.get<Globals::ChainPositions::Peak>();
    auto& highCut = monoChain.get<Globals::ChainPositions::HighCut>();

    std::vector<double> magnitudes;
    magnitudes.resize(boundsWidth);

    for (auto i = 0; i < boundsWidth; ++i)
    {
        double mag = 1.0;
        auto freq = mapToLog10<double>(static_cast<double>(i) / boundsWidth,
                                       Globals::getMinFrequency(),
                                       Globals::getMaxFrequency());

        if (!monoChain.isBypassed<Globals::ChainPositions::Peak>()) {
            mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        if (!lowCut.isBypassed<0>()) mag *= lowCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowCut.isBypassed<1>()) mag *= lowCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowCut.isBypassed<2>()) mag *= lowCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowCut.isBypassed<3>()) mag *= lowCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!highCut.isBypassed<0>()) mag *= highCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highCut.isBypassed<1>()) mag *= highCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highCut.isBypassed<2>()) mag *= highCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highCut.isBypassed<3>()) mag *= highCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        magnitudes.at(i) = juce::Decibels::gainToDecibels(mag);
    }

    auto mapFilterGainRangeToAnalyzerBounds = [&](double magnitude){
        return juce::jmap<float>(magnitude,
                                 Globals::getNegativeInf(),
                                 Globals::getMaxDecibels(),
                                 responseCurveMin,
                                 responseCurveMax);
    };

    juce::Path responseCurveLine;

    responseCurveLine.startNewSubPath(boundsX, mapFilterGainRangeToAnalyzerBounds(magnitudes.at(0)));

    for (size_t i = 1; i < magnitudes.size(); ++i) {
        responseCurveLine.lineTo(boundsX + i, mapFilterGainRangeToAnalyzerBounds(magnitudes.at(i)));
    }

    g.setColour(juce::Colours::black);
    g.strokePath(responseCurveLine, juce::PathStrokeType(2.f));
}

void ResponseCurve::resized()
{
    AnalyzerBase::resized();

    peakNode.setBounds(peakNode.getCoordinates().getX(), peakNode.getCoordinates().getY(), nodeDiameter, nodeDiameter);
}

void ResponseCurve::updateMonoChain()
{
    auto lowCutCoefficients = FilterUtils::makeHighPassFilter(lowCutFreqParam, lowCutSlopeParam, sampleRate);
    auto highCutCoefficients = FilterUtils::makeLowPassFilter(highCutFreqParam, highCutSlopeParam, sampleRate);

    FilterUtils::updateCutCoefficients(monoChain, Globals::ChainPositions::LowCut, lowCutCoefficients, lowCutSlopeParam);
    FilterUtils::updateCutCoefficients(monoChain, Globals::ChainPositions::HighCut, highCutCoefficients, highCutSlopeParam);
    FilterUtils::updatePeakCoefficients(monoChain, peakFreqParam, peakQParam, peakGainParam, sampleRate);
}

void ResponseCurve::updateNodeCoordinates(ResponseCurveNode& node, juce::AudioParameterFloat* freqParam, juce::AudioParameterFloat* gainParam)
{
    auto normalizedFrequency = juce::mapFromLog10<float>(freqParam->get(),
                                                         Globals::getMinFrequency(),
                                                         Globals::getMaxFrequency());

    auto frequencyX = static_cast<int>(std::floor(fftBoundingBox.getX() + fftBoundingBox.getWidth() * normalizedFrequency));

    auto gainY = static_cast<int>(std::floor(juce::jmap<float>(gainParam->get(),
                                                               Globals::getNegativeInf(),
                                                               Globals::getMaxDecibels(),
                                                               fftBoundingBox.getBottom(),
                                                               fftBoundingBox.getY())));

    auto nodeRadius = static_cast<int>(std::floor(nodeDiameter * 0.5));
    node.setCoordinates(frequencyX - nodeRadius, gainY - nodeRadius);
}

void ResponseCurve::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}

void ResponseCurve::timerCallback()
{
    if(parametersChanged.compareAndSetBool(false, true)) {
        updateMonoChain();
        updateNodeCoordinates(peakNode, peakFreqParam, peakGainParam);
        resized();
        repaint();
    }
}

void ResponseCurve::addListeners()
{
    const auto& eqParams = EqProperties::getEqParams();

    apvts.getParameter(eqParams.at(EqProperties::ParamNames::LOW_CUT_FREQ))->addListener(this);
    apvts.getParameter(eqParams.at(EqProperties::ParamNames::LOW_CUT_SLOPE))->addListener(this);
    apvts.getParameter(eqParams.at(EqProperties::ParamNames::HIGH_CUT_FREQ))->addListener(this);
    apvts.getParameter(eqParams.at(EqProperties::ParamNames::HIGH_CUT_SLOPE))->addListener(this);
    apvts.getParameter(eqParams.at(EqProperties::ParamNames::PEAK_FREQ))->addListener(this);
    apvts.getParameter(eqParams.at(EqProperties::ParamNames::PEAK_GAIN))->addListener(this);
    apvts.getParameter(eqParams.at(EqProperties::ParamNames::PEAK_Q))->addListener(this);
}

void ResponseCurve::removeListeners()
{
    const auto& eqParams = EqProperties::getEqParams();

    apvts.getParameter(eqParams.at(EqProperties::ParamNames::LOW_CUT_FREQ))->removeListener(this);
    apvts.getParameter(eqParams.at(EqProperties::ParamNames::LOW_CUT_SLOPE))->removeListener(this);
    apvts.getParameter(eqParams.at(EqProperties::ParamNames::HIGH_CUT_FREQ))->removeListener(this);
    apvts.getParameter(eqParams.at(EqProperties::ParamNames::HIGH_CUT_SLOPE))->removeListener(this);
    apvts.getParameter(eqParams.at(EqProperties::ParamNames::PEAK_FREQ))->removeListener(this);
    apvts.getParameter(eqParams.at(EqProperties::ParamNames::PEAK_GAIN))->removeListener(this);
    apvts.getParameter(eqParams.at(EqProperties::ParamNames::PEAK_Q))->removeListener(this);
}
