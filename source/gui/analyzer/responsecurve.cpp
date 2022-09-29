#include "responsecurve.h"
#include "../../dsp/eqproperties.h"
#include "../../utils/colourpalette.h"

//==============================================================================
Node::Node(juce::AudioProcessorValueTreeState& _apvts, int _bandNum)
: apvts(_apvts)
{
    enabled.referTo(apvts.getParameterAsValue(EqProperties::getPeakControlParamName(EqProperties::PeakControl::ENABLED, _bandNum)));
}

void Node::paint(juce::Graphics& g)
{
    auto colour = enabled.getValue() ? ColourPalette::getColour(ColourPalette::Salmon) : ColourPalette::getColour(ColourPalette::Blue);
    g.setColour(colour);
    g.fillEllipse(getLocalBounds().toFloat());
}

bool Node::isEnabled()
{
    return enabled.getValue();
}

//==============================================================================
ResponseCurve::ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate)
: apvts(_apvts),
  sampleRate(_sampleRate)
{
    for (size_t i = 0; i < Globals::getNumPeakBands(); ++i) {
        peakNodes.at(i) = std::make_unique<Node>(_apvts, i);
    }

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

    for (size_t i = 0; i < Globals::getNumPeakBands(); ++i) {
        assignFloatParam(peakBands.at(i).peakFreqParam, EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, i));
        assignFloatParam(peakBands.at(i).peakGainParam, EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, i));
        assignFloatParam(peakBands.at(i).peakQParam, EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, i));

        addAndMakeVisible(*peakNodes.at(i));

        nodeCoordinates.at(i).setXY(0, 0);

        xValues.at(i).referTo(apvts.getParameterAsValue(EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, i)));
        yValues.at(i).referTo(apvts.getParameterAsValue(EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, i)));
    }

    activeNode = 0;

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
    auto bounds           = getLocalBounds();
    auto boundsWidth      = bounds.getWidth();
    auto boundsX          = bounds.getX();
    auto responseCurveMin = bounds.getBottom();
    auto responseCurveMax = bounds.getY();

    std::vector<double> magnitudes = getMagnitudes(boundsWidth);

    auto mapFilterGainRangeToAnalyzerBounds = [&](double magnitude){
        return juce::jmap<float>(magnitude,
                                 Globals::getNegativeInf(),
                                 Globals::getMaxDecibels(),
                                 responseCurveMin,
                                 responseCurveMax);
    };

    // Drawing the line
    g.setColour(ColourPalette::getColour(ColourPalette::Blue));
    juce::Path responseCurveLine;

    responseCurveLine.startNewSubPath(boundsX, mapFilterGainRangeToAnalyzerBounds(magnitudes.at(0)));

    for (size_t i = 1; i < magnitudes.size(); ++i) {
        responseCurveLine.lineTo(boundsX + i, mapFilterGainRangeToAnalyzerBounds(magnitudes.at(i)));
    }

    g.strokePath(responseCurveLine, juce::PathStrokeType(2.f));

    // Drawing the nodes
    for (size_t i = 0; i < peakNodes.size(); ++i) {
        auto normalizedFrequency = juce::mapFromLog10<float>(peakBands.at(i).peakFreqParam->get(),
                                                             Globals::getMinFrequency(),
                                                             Globals::getMaxFrequency());

        auto nodeX = boundsX + static_cast<int>(std::floor(boundsWidth * normalizedFrequency));
        auto nodeY = mapFilterGainRangeToAnalyzerBounds(magnitudes.at(nodeX - boundsX));

        peakNodes.at(i)->setBounds(nodeX - nodeRadius, nodeY - nodeRadius, nodeDiameter, nodeDiameter);
        nodeCoordinates.at(i).setXY(nodeX, static_cast<int>(std::floor(nodeY)));
    }
}

void ResponseCurve::mouseDrag(const juce::MouseEvent& event)
{
    auto xCoord = event.position.getX();
    auto yCoord = event.position.getY();

    auto bounds       = getLocalBounds();
    auto boundsWidth  = bounds.getWidth();
    auto boundsHeight = bounds.getHeight();

    if (xCoord >= 0 && xCoord <= boundsWidth && yCoord >= 0 && yCoord <= boundsHeight) {
        auto xFrequency = mapToLog10<float>(static_cast<float>(xCoord) / boundsWidth,
                                            Globals::getMinFrequency(),
                                            Globals::getMaxFrequency());

        auto yDecibels = juce::jmap<float>(yCoord,
                                           0,
                                           boundsHeight,
                                           Globals::getMaxDecibels(),
                                           Globals::getNegativeInf());

        size_t closestNode = 0;
        auto shortestDistance = boundsWidth;
        for (size_t i = 0; i < nodeCoordinates.size(); ++i) {
            auto xDistance = std::abs(nodeCoordinates.at(i).getX() - xCoord);
            if (xDistance < shortestDistance) {
                closestNode = i;
                shortestDistance = xDistance;
            }
        }

        if (peakNodes.at(closestNode)->isEnabled()) {
            xValues.at(closestNode) = xFrequency;
            yValues.at(closestNode) = yDecibels;

            activeNode = closestNode;
        }
    }
}

void ResponseCurve::updateMonoChain()
{
    auto lowCutCoefficients  = FilterUtils::makeHighPassFilter(lowCutFreqParam, lowCutSlopeParam, sampleRate);
    auto highCutCoefficients = FilterUtils::makeLowPassFilter(highCutFreqParam, highCutSlopeParam, sampleRate);

    FilterUtils::updateCutCoefficients(monoChain, FilterUtils::ChainPositions::LowCut, lowCutCoefficients, lowCutSlopeParam);
    FilterUtils::updateCutCoefficients(monoChain, FilterUtils::ChainPositions::HighCut, highCutCoefficients, highCutSlopeParam);

    for (size_t i = 0; i < peakBands.size(); ++i) {
        FilterUtils::updatePeakCoefficients(monoChain, static_cast<FilterUtils::ChainPositions>(i+1), peakBands.at(i).peakFreqParam, peakBands.at(i).peakQParam, peakBands.at(i).peakGainParam, sampleRate);
    }
}

std::vector<double> ResponseCurve::getMagnitudes(int boundsWidth)
{
    std::vector<double> magnitudes;
    magnitudes.resize(boundsWidth);

    auto& lowCut  = monoChain.get<FilterUtils::ChainPositions::LowCut>();
    auto& peak0   = monoChain.get<FilterUtils::ChainPositions::Peak_0>();
    auto& peak1   = monoChain.get<FilterUtils::ChainPositions::Peak_1>();
    auto& peak2   = monoChain.get<FilterUtils::ChainPositions::Peak_2>();
    auto& peak3   = monoChain.get<FilterUtils::ChainPositions::Peak_3>();
    auto& peak4   = monoChain.get<FilterUtils::ChainPositions::Peak_4>();
    auto& peak5   = monoChain.get<FilterUtils::ChainPositions::Peak_5>();
    auto& highCut = monoChain.get<FilterUtils::ChainPositions::HighCut>();

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

    return magnitudes;
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
