#include "responsecurve.h"
#include "../dsp/eqproperties.h"
#include "../utils/colourpalette.h"

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

    const auto& eqParams = EqProperties::getCutParams();
    assignFloatParam(lowCutFreqParam, eqParams.at(EqProperties::CutControls::LOW_CUT_FREQ));
    assignChoiceParam(lowCutSlopeParam, eqParams.at(EqProperties::CutControls::LOW_CUT_SLOPE));
    assignFloatParam(highCutFreqParam, eqParams.at(EqProperties::CutControls::HIGH_CUT_FREQ));
    assignChoiceParam(highCutSlopeParam, eqParams.at(EqProperties::CutControls::HIGH_CUT_SLOPE));

    for (size_t i = 0; i < peakBands.size(); ++i) {
        assignFloatParam(peakBands.at(i).peakFreqParam, EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, i));
        assignFloatParam(peakBands.at(i).peakGainParam, EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, i));
        assignFloatParam(peakBands.at(i).peakQParam, EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, i));
    }

    for (size_t i = 0; i < peakNodes.size(); ++i) {
        addAndMakeVisible(peakNodes.at(i));
    }

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

    auto& lowCut = monoChain.get<FilterUtils::ChainPositions::LowCut>();
    auto& peak0 = monoChain.get<FilterUtils::ChainPositions::Peak_0>();
    auto& peak1 = monoChain.get<FilterUtils::ChainPositions::Peak_1>();
    auto& peak2 = monoChain.get<FilterUtils::ChainPositions::Peak_2>();
    auto& peak3 = monoChain.get<FilterUtils::ChainPositions::Peak_3>();
    auto& peak4 = monoChain.get<FilterUtils::ChainPositions::Peak_4>();
    auto& peak5 = monoChain.get<FilterUtils::ChainPositions::Peak_5>();
    auto& highCut = monoChain.get<FilterUtils::ChainPositions::HighCut>();

    std::vector<double> magnitudes;
    magnitudes.resize(boundsWidth);

    for (auto i = 0; i < boundsWidth; ++i)
    {
        double mag = 1.0;
        auto freq = mapToLog10<double>(static_cast<double>(i) / boundsWidth,
                                       Globals::getMinFrequency(),
                                       Globals::getMaxFrequency());

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::Peak_0>()) {
            mag *= peak0.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::Peak_1>()) {
            mag *= peak1.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::Peak_2>()) {
            mag *= peak2.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::Peak_3>()) {
            mag *= peak3.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::Peak_4>()) {
            mag *= peak4.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::Peak_5>()) {
            mag *= peak5.coefficients->getMagnitudeForFrequency(freq, sampleRate);
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

    g.setColour(ColourPalette::getColour(ColourPalette::Blue));
    g.strokePath(responseCurveLine, juce::PathStrokeType(2.f));

    for (size_t i = 0; i < peakNodes.size(); ++i) {
        auto normalizedFrequency = juce::mapFromLog10<float>(peakBands.at(i).peakFreqParam->get(),
                                                             Globals::getMinFrequency(),
                                                             Globals::getMaxFrequency());

        auto nodeX = static_cast<int>(std::floor(boundsX + boundsWidth * normalizedFrequency));
        auto nodeY = mapFilterGainRangeToAnalyzerBounds(magnitudes.at(nodeX));
        auto nodeRadius = static_cast<int>(std::floor(nodeDiameter * 0.5));

        peakNodes.at(i).setBounds(nodeX - nodeRadius, nodeY - nodeRadius, nodeDiameter, nodeDiameter);
    }
}

void ResponseCurve::resized()
{
    AnalyzerBase::resized();
}

void ResponseCurve::updateMonoChain()
{
    auto lowCutCoefficients = FilterUtils::makeHighPassFilter(lowCutFreqParam, lowCutSlopeParam, sampleRate);
    auto highCutCoefficients = FilterUtils::makeLowPassFilter(highCutFreqParam, highCutSlopeParam, sampleRate);

    FilterUtils::updateCutCoefficients(monoChain, FilterUtils::ChainPositions::LowCut, lowCutCoefficients, lowCutSlopeParam);
    FilterUtils::updateCutCoefficients(monoChain, FilterUtils::ChainPositions::HighCut, highCutCoefficients, highCutSlopeParam);

    for (size_t i = 0; i < peakBands.size(); ++i) {
        FilterUtils::updatePeakCoefficients(monoChain, static_cast<FilterUtils::ChainPositions>(i+1), peakBands.at(i).peakFreqParam, peakBands.at(i).peakQParam, peakBands.at(i).peakGainParam, sampleRate);
    }
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
        repaint();
    }
}

void ResponseCurve::addListeners()
{
    const auto& eqParams = EqProperties::getCutParams();

    apvts.getParameter(eqParams.at(EqProperties::CutControls::LOW_CUT_FREQ))->addListener(this);
    apvts.getParameter(eqParams.at(EqProperties::CutControls::LOW_CUT_SLOPE))->addListener(this);
    apvts.getParameter(eqParams.at(EqProperties::CutControls::HIGH_CUT_FREQ))->addListener(this);
    apvts.getParameter(eqParams.at(EqProperties::CutControls::HIGH_CUT_SLOPE))->addListener(this);

    for (size_t i = 0; i < peakBands.size(); ++i) {
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, i))->addListener(this);
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, i))->addListener(this);
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, i))->addListener(this);
    }
}

void ResponseCurve::removeListeners()
{
    const auto& eqParams = EqProperties::getCutParams();

    apvts.getParameter(eqParams.at(EqProperties::CutControls::LOW_CUT_FREQ))->removeListener(this);
    apvts.getParameter(eqParams.at(EqProperties::CutControls::LOW_CUT_SLOPE))->removeListener(this);
    apvts.getParameter(eqParams.at(EqProperties::CutControls::HIGH_CUT_FREQ))->removeListener(this);
    apvts.getParameter(eqParams.at(EqProperties::CutControls::HIGH_CUT_SLOPE))->removeListener(this);

    for (size_t i = 0; i < peakBands.size(); ++i) {
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, i))->removeListener(this);
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, i))->removeListener(this);
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, i))->removeListener(this);
    }
}
