#include "responsecurve.h"
#include "../utils/globals.h"

//==============================================================================
ResponseCurve::ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate)
: apvts(_apvts),
  sampleRate(_sampleRate)
{
    apvts.getParameter("LowCutFreq")->addListener(this);
    apvts.getParameter("LowCutSlope")->addListener(this);
    apvts.getParameter("HighCutFreq")->addListener(this);
    apvts.getParameter("HighCutSlope")->addListener(this);
    apvts.getParameter("PeakFreq")->addListener(this);
    apvts.getParameter("PeakGain")->addListener(this);
    apvts.getParameter("PeakQ")->addListener(this);

    updateMonoChain();
}

ResponseCurve::~ResponseCurve()
{
    apvts.getParameter("LowCutFreq")->removeListener(this);
    apvts.getParameter("LowCutSlope")->removeListener(this);
    apvts.getParameter("HighCutFreq")->removeListener(this);
    apvts.getParameter("HighCutSlope")->removeListener(this);
    apvts.getParameter("PeakFreq")->removeListener(this);
    apvts.getParameter("PeakGain")->removeListener(this);
    apvts.getParameter("PeakQ")->removeListener(this);
}

void ResponseCurve::paint(juce::Graphics& g)
{
    auto analyzerBounds = getLocalBounds();
    auto analyzerWidth = analyzerBounds.getWidth();
    auto analyzerLeft = analyzerBounds.getX();
    auto responseCurveMin = analyzerBounds.getBottom();
    auto responseCurveMax = analyzerBounds.getY();

    auto& lowCut = monoChain.get<Globals::ChainPositions::LowCut>();
    auto& peak = monoChain.get<Globals::ChainPositions::Peak>();
    auto& highCut = monoChain.get<Globals::ChainPositions::HighCut>();

    std::vector<double> magnitudes;
    magnitudes.resize(analyzerWidth);

    for (auto i = 0; i < analyzerWidth; ++i)
    {
        double mag = 1.0;
        auto freq = mapToLog10<double>(static_cast<double>(i) / analyzerWidth,
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

    juce::Path line;

    line.startNewSubPath(analyzerLeft, mapFilterGainRangeToAnalyzerBounds(magnitudes.at(0)));

    for (size_t i = 1; i < magnitudes.size(); ++i) {
        line.lineTo(analyzerLeft + i, mapFilterGainRangeToAnalyzerBounds(magnitudes.at(i)));
    }

    line.closeSubPath();

    g.setColour(juce::Colours::black);

    g.strokePath(line, juce::PathStrokeType(2.f));
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

    assignFloatParam(lowCutFreqParam, "LowCutFreq");
    assignChoiceParam(lowCutSlopeParam, "LowCutSlope");

    assignFloatParam(highCutFreqParam, "HighCutFreq");
    assignChoiceParam(highCutSlopeParam, "HighCutSlope");

    assignFloatParam(peakFreqParam, "PeakFreq");
    assignFloatParam(peakGainParam, "PeakGain");
    assignFloatParam(peakQParam, "PeakQ");

    auto lowCutCoefficients = FilterUtils::makeHighPassFilter(lowCutFreqParam, lowCutSlopeParam, sampleRate);
    auto highCutCoefficients = FilterUtils::makeLowPassFilter(highCutFreqParam, highCutSlopeParam, sampleRate);

    FilterUtils::updateCutCoefficients(monoChain, Globals::ChainPositions::LowCut, lowCutCoefficients, lowCutSlopeParam);
    FilterUtils::updateCutCoefficients(monoChain, Globals::ChainPositions::HighCut, highCutCoefficients, highCutSlopeParam);
    FilterUtils::updatePeakCoefficients(monoChain, peakFreqParam, peakQParam, peakGainParam, sampleRate);

    repaint();
}

void ResponseCurve::parameterValueChanged(int parameterIndex, float newValue)
{
    DBG("Parameter: " << parameterIndex << " changed to: " << newValue);
    updateMonoChain();
}
