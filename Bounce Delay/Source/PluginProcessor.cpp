/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BounceDelayAudioProcessor::BounceDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    mParamTree(*this,nullptr,"PARAMETERS",createParameterLayout())
#endif
{
}

BounceDelayAudioProcessor::~BounceDelayAudioProcessor()
{
}

//==============================================================================
const juce::String BounceDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BounceDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BounceDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BounceDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BounceDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BounceDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BounceDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BounceDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BounceDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void BounceDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BounceDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec mDelaySpec = { sampleRate,samplesPerBlock,2 };
    mDelay= juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>(sampleRate *10);
    mDelay.prepare(mDelaySpec);

}

void BounceDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BounceDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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
#endif

void BounceDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());



    auto delayTimeInMillis = mParamTree.getRawParameterValue("DELAYID")->load();
    int n_bounces = mParamTree.getRawParameterValue("NUMBERBOUNCESID")->load();
    auto dw = mParamTree.getRawParameterValue("DRYWETID")->load();
    float elasticity = mParamTree.getRawParameterValue("ELASTICITYID")->load();
    float frequency_decay = mParamTree.getRawParameterValue("FREQUENCYID")->load();
    int stereo = mParamTree.getRawParameterValue("STEREOID")->load();
    int reverse = mParamTree.getRawParameterValue("REVERSEID")->load();
    int multiplier = (reverse == 0) ? 1 : -1;

    int delayTimeInSamples =(int) delayTimeInMillis *0.001* getSampleRate();



    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            float dryval = channelData[sample];
            float wetval = 0;
            mDelay.pushSample(channel, dryval); 

            for (int i = stereo*channel; i < n_bounces; i+=1 + stereo) {
                float amplitude = std::pow(elasticity, (n_bounces*reverse) +multiplier*(i+1));  

                wetval +=amplitude* mDelay.popSample(channel, delayTimeInSamples*(1.0f-std::pow(frequency_decay,i))/(1.0f-frequency_decay), false);
            }
            mDelay.popSample(channel, -1, true);            
            channelData[sample] = dryval * (1.0f - dw) + dw * wetval;

            

        }

        
    }
}

//==============================================================================
bool BounceDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BounceDelayAudioProcessor::createEditor()
{
    return new BounceDelayAudioProcessorEditor (*this);
}

//==============================================================================
void BounceDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = mParamTree.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
   
}

void BounceDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if(xmlState.get()!=nullptr)
        if (xmlState->hasTagName(mParamTree.state.getType()))
            mParamTree.replaceState(juce::ValueTree::fromXml(*xmlState));

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BounceDelayAudioProcessor();
}


juce::AudioProcessorValueTreeState::ParameterLayout BounceDelayAudioProcessor::createParameterLayout()
{

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    //in Hzs
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("DELAYID", "DELAY", 1.0f, 500.0f, 50.0f));
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("DRYWETID", "DRYWET", 0.0f, 1.0f, 0.3f));
    params.emplace_back(std::make_unique<juce::AudioParameterInt>("NUMBERBOUNCESID", "NUMBERBOUNCES", 1,50,24));
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("ELASTICITYID", "ELASTICITY", 0.0f, 1.0f, 0.5f));
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("FREQUENCYID", "FREQUENCY", 0.8f, 0.95f, 0.9f));
    params.emplace_back(std::make_unique<juce::AudioParameterBool>("STEREOID", "STEREO", true));

    params.emplace_back(std::make_unique<juce::AudioParameterBool>("REVERSEID", "REVERSE", false));

    return { params.begin(),params.end() };

}


