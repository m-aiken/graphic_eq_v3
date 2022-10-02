#include "responsecurve.h"
#include "../../dsp/eqproperties.h"
#include "../../utils/colourpalette.h"

//==============================================================================
Node::Node(juce::AudioProcessorValueTreeState& _apvts, int _bandNum)
{
    auto paramId = EqProperties::getPeakControlParamName(EqProperties::PeakControl::ENABLED, _bandNum);
    paramAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(_apvts, paramId, *this);
}

void Node::paint(juce::Graphics& g)
{
    auto colour = getToggleState() ? ColourPalette::getColour(ColourPalette::Salmon) : ColourPalette::getColour(ColourPalette::Blue);
    g.setColour(colour);
    g.fillEllipse(getLocalBounds().toFloat());
}

//==============================================================================
ResponseCurve::ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate)
: sampleRate(_sampleRate),
  apvts(_apvts)
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

    std::array<std::vector<double>, 9> magnitudes = getMagnitudes(boundsWidth);
    std::array<juce::Path, 9> paths;

    auto mapFilterGainRangeToAnalyzerBounds = [&](double magnitude){
        return juce::jmap<float>(magnitude,
                                 Globals::getNegativeInf(),
                                 Globals::getMaxDecibels(),
                                 responseCurveMin,
                                 responseCurveMax);
    };

    // Plotting the paths for each band
    // These need to be closed paths starting and ending at 0 for the fill effect to look correct
    for (size_t bandIdx = 0; bandIdx < paths.size() - 1; ++bandIdx) {
        paths.at(bandIdx).startNewSubPath(boundsX, mapFilterGainRangeToAnalyzerBounds(0.0));

        for (int i = 1; i < boundsWidth - 1; ++i) {
            paths.at(bandIdx).lineTo(boundsX + i, mapFilterGainRangeToAnalyzerBounds(magnitudes.at(bandIdx).at(i)));
        }

        paths.at(bandIdx).lineTo(boundsWidth - 1, mapFilterGainRangeToAnalyzerBounds(0.0));
        paths.at(bandIdx).closeSubPath();
    }

    // Drawing the bolder outline
    g.setColour(ColourPalette::getColour(ColourPalette::Salmon));
    for (size_t pathIdx = 0; pathIdx < paths.size() - 1; ++pathIdx) {
        g.strokePath(paths.at(pathIdx), juce::PathStrokeType(1.f));
    }

    // Drawing the faded fill
    g.setColour(ColourPalette::getColour(ColourPalette::Salmon).withAlpha(0.2f));
    for (size_t pathIdx = 0; pathIdx < paths.size() - 1; ++pathIdx) {
        g.fillPath(paths.at(pathIdx));
    }

    // Plotting the path for total filter response
    // This should NOT be closed so that it's just a single line
    for (size_t bandIdx = 0; bandIdx < paths.size(); ++bandIdx) {
        paths.at(bandIdx).startNewSubPath(boundsX, mapFilterGainRangeToAnalyzerBounds(magnitudes.at(bandIdx).at(0)));

        for (int i = 1; i < boundsWidth; ++i) {
            paths.at(bandIdx).lineTo(boundsX + i, mapFilterGainRangeToAnalyzerBounds(magnitudes.at(bandIdx).at(i)));
        }
    }

    g.setColour(ColourPalette::getColour(ColourPalette::Blue));
    g.strokePath(paths.at(paths.size() - 1), juce::PathStrokeType(2.f));

    // Drawing the nodes
    for (size_t i = 0; i < peakNodes.size(); ++i) {
        auto normalizedFrequency = juce::mapFromLog10<float>(peakBands.at(i).peakFreqParam->get(),
                                                             Globals::getMinFrequency(),
                                                             Globals::getMaxFrequency());

        auto nodeX = boundsX + static_cast<int>(std::floor(boundsWidth * normalizedFrequency));
        auto nodeY = mapFilterGainRangeToAnalyzerBounds(magnitudes.at(i).at(nodeX - boundsX));

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

        if (peakNodes.at(closestNode)->getToggleState()) {
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

std::array<std::vector<double>, 9> ResponseCurve::getMagnitudes(int boundsWidth)
{
    std::array<std::vector<double>, 9> magnitudes;
    for (auto& mag : magnitudes) {
        mag.resize(boundsWidth);
    }

    enum MagIdx {
        P0, P1, P2, P3, P4, P5, LC, HC, ALL
    };

    auto& peak0   = monoChain.get<FilterUtils::ChainPositions::Peak_0>();
    auto& peak1   = monoChain.get<FilterUtils::ChainPositions::Peak_1>();
    auto& peak2   = monoChain.get<FilterUtils::ChainPositions::Peak_2>();
    auto& peak3   = monoChain.get<FilterUtils::ChainPositions::Peak_3>();
    auto& peak4   = monoChain.get<FilterUtils::ChainPositions::Peak_4>();
    auto& peak5   = monoChain.get<FilterUtils::ChainPositions::Peak_5>();
    auto& lowCut  = monoChain.get<FilterUtils::ChainPositions::LowCut>();
    auto& highCut = monoChain.get<FilterUtils::ChainPositions::HighCut>();

    for (auto i = 0; i < boundsWidth; ++i) {
        double magP0  = 1.0;
        double magP1  = 1.0;
        double magP2  = 1.0;
        double magP3  = 1.0;
        double magP4  = 1.0;
        double magP5  = 1.0;
        double magLC  = 1.0;
        double magHC  = 1.0;
        double magALL = 1.0;

        auto freq = mapToLog10<double>(static_cast<double>(i) / boundsWidth,
                                       Globals::getMinFrequency(),
                                       Globals::getMaxFrequency());

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::Peak_0>()) {
            magP0  *= peak0.coefficients->getMagnitudeForFrequency(freq, sampleRate);
            magALL *= peak0.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::Peak_1>()) {
            magP1  *= peak1.coefficients->getMagnitudeForFrequency(freq, sampleRate);
            magALL *= peak1.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::Peak_2>()) {
            magP2  *= peak2.coefficients->getMagnitudeForFrequency(freq, sampleRate);
            magALL *= peak2.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::Peak_3>()) {
            magP3  *= peak3.coefficients->getMagnitudeForFrequency(freq, sampleRate);
            magALL *= peak3.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::Peak_4>()) {
            magP4  *= peak4.coefficients->getMagnitudeForFrequency(freq, sampleRate);
            magALL *= peak4.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::Peak_5>()) {
            magP5  *= peak5.coefficients->getMagnitudeForFrequency(freq, sampleRate);
            magALL *= peak5.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::LowCut>()) {
            if (!lowCut.isBypassed<0>()) {
                magHC *= lowCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= lowCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (!lowCut.isBypassed<1>()) {
                magHC *= lowCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= lowCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (!lowCut.isBypassed<2>()) {
                magHC *= lowCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= lowCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (!lowCut.isBypassed<3>()) {
                magHC *= lowCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= lowCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
        }

        if (!monoChain.isBypassed<FilterUtils::ChainPositions::HighCut>()) {
            if (!highCut.isBypassed<0>()) {
                magHC *= highCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= highCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (!highCut.isBypassed<1>()) {
                magHC *= highCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= highCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (!highCut.isBypassed<2>()) {
                magHC *= highCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= highCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (!highCut.isBypassed<3>()) {
                magHC *= highCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= highCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
        }

        magnitudes.at(MagIdx::P0).at(i)  = juce::Decibels::gainToDecibels(magP0);
        magnitudes.at(MagIdx::P1).at(i)  = juce::Decibels::gainToDecibels(magP1);
        magnitudes.at(MagIdx::P2).at(i)  = juce::Decibels::gainToDecibels(magP2);
        magnitudes.at(MagIdx::P3).at(i)  = juce::Decibels::gainToDecibels(magP3);
        magnitudes.at(MagIdx::P4).at(i)  = juce::Decibels::gainToDecibels(magP4);
        magnitudes.at(MagIdx::P5).at(i)  = juce::Decibels::gainToDecibels(magP5);
        magnitudes.at(MagIdx::LC).at(i)  = juce::Decibels::gainToDecibels(magLC);
        magnitudes.at(MagIdx::HC).at(i)  = juce::Decibels::gainToDecibels(magHC);
        magnitudes.at(MagIdx::ALL).at(i) = juce::Decibels::gainToDecibels(magALL);
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
