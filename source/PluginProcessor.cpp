#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/analyzerproperties.h"

//==============================================================================
GraphicEqProcessor::GraphicEqProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

GraphicEqProcessor::~GraphicEqProcessor()
{
}

//==============================================================================
const juce::String GraphicEqProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GraphicEqProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GraphicEqProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GraphicEqProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GraphicEqProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GraphicEqProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GraphicEqProcessor::getCurrentProgram()
{
    return 0;
}

void GraphicEqProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String GraphicEqProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void GraphicEqProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void GraphicEqProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need.
    juce::ignoreUnused (sampleRate);
    /*
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    */

    lScsf.prepare(samplesPerBlock);
    rScsf.prepare(samplesPerBlock);
}

void GraphicEqProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool GraphicEqProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void GraphicEqProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    lScsf.update(buffer);
    rScsf.update(buffer);
}

//==============================================================================
bool GraphicEqProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GraphicEqProcessor::createEditor()
{
    return new GraphicEqEditor (*this);
}

//==============================================================================
void GraphicEqProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos (destData, true);
    apvts.state.writeToStream(mos);
}

void GraphicEqProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if ( tree.isValid() )
    {
        apvts.replaceState(tree);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout GraphicEqProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    juce::StringArray cutChoices;

    for (auto i = 0; i < 4; ++i)
    {
        juce::String str;
        str << (12 + i*12);
        str << " dB/Oct";
        cutChoices.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCutFreq",
                                                           "Low Cut Freq",
                                                           juce::NormalisableRange(20.f, 20000.f, 1.f, 1.f),
                                                           20.f));

    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCutSlope",
                                                            "Low Cut Slope",
                                                            cutChoices,
                                                            0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("HighCutFreq",
                                                           "High Cut Freq",
                                                           juce::NormalisableRange(20.f, 20000.f, 1.f, 1.f),
                                                           20000.f));

    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCutSlope",
                                                            "Low Cut Slope",
                                                            cutChoices,
                                                            0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakFreq",
                                                           "Peak Freq",
                                                           juce::NormalisableRange(20.f, 20000.f, 1.f, 1.f),
                                                           750.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakGain",
                                                           "Peak Gain",
                                                           juce::NormalisableRange(-24.f, 24.f, 0.5f, 1.f),
                                                           0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakQ",
                                                           "Peak Q",
                                                           juce::NormalisableRange(0.1f, 10.f, 0.05f, 1.f),
                                                           1.f));

    AnalyzerProperties::addAnalyzerParams(layout);
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GraphicEqProcessor();
}
