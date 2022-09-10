#include "responsecurve.h"
#include "../utils/globals.h"
#include "../dsp/eqproperties.h"

//==============================================================================
ResponseCurve::ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate)
: apvts(_apvts),
  sampleRate(_sampleRate)
{
    addListeners();
    updateMonoChain();
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

void ResponseCurve::updateMonoChain()
{
    juce::AudioParameterFloat*  lowCutFreqParam   { nullptr };
    juce::AudioParameterChoice* lowCutSlopeParam  { nullptr };

    juce::AudioParameterFloat*  highCutFreqParam  { nullptr };
    juce::AudioParameterChoice* highCutSlopeParam { nullptr };

    juce::AudioParameterFloat*  peakFreqParam     { nullptr };
    juce::AudioParameterFloat*  peakGainParam     { nullptr };
    juce::AudioParameterFloat*  peakQParam        { nullptr };

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

    auto lowCutCoefficients = FilterUtils::makeHighPassFilter(lowCutFreqParam, lowCutSlopeParam, sampleRate);
    auto highCutCoefficients = FilterUtils::makeLowPassFilter(highCutFreqParam, highCutSlopeParam, sampleRate);

    FilterUtils::updateCutCoefficients(monoChain, Globals::ChainPositions::LowCut, lowCutCoefficients, lowCutSlopeParam);
    FilterUtils::updateCutCoefficients(monoChain, Globals::ChainPositions::HighCut, highCutCoefficients, highCutSlopeParam);
    FilterUtils::updatePeakCoefficients(monoChain, peakFreqParam, peakQParam, peakGainParam, sampleRate);

    repaint();
}

void ResponseCurve::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}

void ResponseCurve::timerCallback()
{
    if(parametersChanged.compareAndSetBool(false, true)) {
        updateMonoChain();
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
