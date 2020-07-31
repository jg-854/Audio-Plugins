/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChorusAudioProcessor::ChorusAudioProcessor()
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

ChorusAudioProcessor::~ChorusAudioProcessor()
{
}

//==============================================================================
const juce::String ChorusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChorusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChorusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChorusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChorusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChorusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ChorusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChorusAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ChorusAudioProcessor::getProgramName (int index)
{
    return {};
}

void ChorusAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ChorusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mSampleRate = sampleRate;
    //prepare lfo
    juce::dsp::ProcessSpec lfoSpec = { sampleRate,samplesPerBlock,2 };

    //prepare delay line
    
    juce::dsp::ProcessSpec delayLineSpec = { sampleRate,samplesPerBlock,getMainBusNumOutputChannels() };   

    for (int i = 0; i < numDelayLines; ++i) {
        lfosleft[i].prepare(lfoSpec);
        lfosright[i].prepare(lfoSpec);
        lfosleft[i].initialise([](float x) {return std::sin(x); }, 128);
        lfosright[i].initialise([](float x) {return std::sin(x); }, 128);
        mDelayLines[i] = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>(sampleRate * MAXDELAY);
        mDelayLines[i].prepare(delayLineSpec);
    }

    //prepare highpass filter and set cutoff freq
    
    for (int i = 0; i < 2; ++i)
    {
        mHighPassFilter[i].prepare(lfoSpec); //same spec as lfo ;
        mHighPassFilter[i].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, mHighpashCutoff);
    }


   




}

void ChorusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChorusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ChorusAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples()); 

    ////////////////////// MY CODE //////////////////////////

    //retrieve parameters from gui

    float wet = mParamTree.getRawParameterValue("DRYWETID")->load();
    float depth = mParamTree.getRawParameterValue("LFODEPTHID")->load(); //in milliseconds
    float lforate = mParamTree.getRawParameterValue("LFORATEID")->load();
   float feedback = mParamTree.getRawParameterValue("FEEDBACKID")->load();
    //derived quantities 

    float  depthInSamples = depth * 0.001 * mSampleRate;
    //TODO : incorporate second LFo to the second delay line rather than both delay lines modulted by the same lfo
    //update lfo and retrieve the phase offset for the delay

    lfosleft[0].setFrequency(lforate);    
    lfosright[0].setFrequency(lforate); 
    lfosleft[1].setFrequency(lforate*1.2);
    lfosright[1].setFrequency(lforate*1.2);//changes things up right

    juce::dsp::Oscillator<float>* lfo;
    float instantaneousOffsetInSamples[2];

    
    for (int channel = 0; channel < 2; ++channel)    {
        if (channel == 0) lfo = lfosleft;
        else lfo = lfosright;
  
        auto* BufferWritePtr = buffer.getWritePointer(channel);
        
        

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            
            auto drySignal = BufferWritePtr[sample];
            

            //update lfo and retrieve signal from delay buffer, the max function is to ensure our delay is never negative
            

            float result = 0;
            for (int i = 0; i < numDelayLines; ++i) {
                instantaneousOffsetInSamples[i] = depthInSamples * lfo[i].processSample(0.0f);
                float instanteousDelayedSignal = mDelayLines[i].popSample(channel, delayTimes[i]*mSampleRate + instantaneousOffsetInSamples[i]);
                //push dry signal into delaybuffers and feedback
                mDelayLines[i].pushSample(channel, drySignal + feedback*instanteousDelayedSignal);
                
                result +=abs(channel-panValues[i])* instanteousDelayedSignal; //pan some of the signals
            }

            //highpass the wet signal

            result = mHighPassFilter[channel].processSample(result);

            //combine wet and dry signal
            BufferWritePtr[sample] = (wet * result) + (1.0f - wet)*drySignal;           
            
            
        }

        
    }
    offsetToEditor1 = instantaneousOffsetInSamples[0];
    offsetToEditor2 = instantaneousOffsetInSamples[1];
}

//==============================================================================
bool ChorusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ChorusAudioProcessor::createEditor()
{
    return new ChorusAudioProcessorEditor (*this);
}

//==============================================================================
void ChorusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ChorusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChorusAudioProcessor();
}


juce::AudioProcessorValueTreeState::ParameterLayout ChorusAudioProcessor::createParameterLayout()
{

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    //in Hzs
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("LFORATEID", "LFO RATE", 0.03f, 10.0f, 0.1f));

    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("LFODEPTHID", "LFO DEPTH", 0.0f, 6.50f, 5.50f));

    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("DRYWETID", "DRY/WET", 0.0f, 1.0f, 0.5f));

    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("FEEDBACKID", "FEEDBACK", 0.0f, 0.950f, 0.0f));


    return { params.begin(),params.end() };
    
}