#pragma once

#include <JuceHeader.h>

#include "analyzerbase.h"
#include "pathproducer.h"
#include "dbscale.h"
#include "paramlistener.h"
#include "../dsp/singlesamplefifo.h"
#include "../utils/globals.h"

//==============================================================================
struct SpectrumAnalyzer : AnalyzerBase, juce::Timer
{
    SpectrumAnalyzer(double _sampleRate,
                     SingleChannelSampleFifo<juce::AudioBuffer<float>>& leftScsf,
                     SingleChannelSampleFifo<juce::AudioBuffer<float>>& rightScsf,
                     juce::AudioProcessorValueTreeState& apvts);

    void timerCallback() override;
    void resized() override;
    void paint(juce::Graphics& g) override;
    void customizeScales(int lsMin, int lsMax, int rsMin, int rsMax, int division);
private:
    double sampleRate;
    juce::Path leftAnalyzerPath, rightAnalyzerPath;

    PathProducer leftPathProducer, rightPathProducer;

//    bool active { false };
    bool active { true }; // TODO for test only

    void paintBackground(juce::Graphics& g);

    void setActive(bool activeState);
    void updateDecayRate(float decayRate);
    void updateOrder(float value);
    void animate();

    DbScale analyzerScale, eqScale;
    /*
    std::unique_ptr<ParamListener<float>> analyzerEnabledParamListener,
    analyzerDecayRateParamListener,
    analyzerOrderParamListener;
    */
    float leftScaleMin  {Globals::getNegativeInf()},
         leftScaleMax  {Globals::getMaxDecibels()},
         rightScaleMin {Globals::getNegativeInf()},
         rightScaleMax {Globals::getMaxDecibels()};

    int scaleDivision { 12 };
};
