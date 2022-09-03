#pragma once

#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "dsp/singlesamplefifo.h"
#include "utils/globals.h"

//==============================================================================
class GraphicEqProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    GraphicEqProcessor();
    ~GraphicEqProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };

    SingleChannelSampleFifo<juce::AudioBuffer<float>> lScsf { Globals::Channel::Left };
    SingleChannelSampleFifo<juce::AudioBuffer<float>> rScsf { Globals::Channel::Right };
private:

    using Filter = juce::dsp::IIR::Filter<float>;

    // Cut Filter chain: x4 Filters to allow for 12/24/36/48 dB/oct options
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

    /*
     * Mono chain
     * currently a 3 band EQ - TODO add more bands
     * For 3 band we need:
     *     LowCut,    Peak,   HighCut
     * aka
     *     CutFilter, Filter, CutFilter
    */

    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

    enum ChainPositions
    {
        LowCut,
        Peak,
        HighCut
    };

    enum Slope
    {
        Slope_12,
        Slope_24,
        Slope_36,
        Slope_48
    };

    MonoChain leftChain, rightChain;

    juce::AudioParameterFloat*  lowCutFreqParam   { nullptr };
    juce::AudioParameterChoice* lowCutSlopeParam  { nullptr };
    juce::AudioParameterFloat*  highCutFreqParam  { nullptr };
    juce::AudioParameterChoice* highCutSlopeParam { nullptr };
    juce::AudioParameterFloat*  peakFreqParam     { nullptr };
    juce::AudioParameterFloat*  peakGainParam     { nullptr };
    juce::AudioParameterFloat*  peakQParam        { nullptr };

    void updatePeakCoefficients(double sampleRate);
    void updateCutCoefficients(juce::ReferenceCountedArray<juce::dsp::FilterDesign<float>::IIRCoefficients>& coefficients, juce::AudioParameterChoice* slopeParam);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphicEqProcessor)
};
