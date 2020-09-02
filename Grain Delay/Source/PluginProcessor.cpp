/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GrainDelayAudioProcessor::GrainDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
     ),
    mParamTree(*this,nullptr,"Parameters",createParameterLayout())
#endif
{
}

GrainDelayAudioProcessor::~GrainDelayAudioProcessor()
{
}

//==============================================================================
const juce::String GrainDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GrainDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GrainDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GrainDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GrainDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GrainDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GrainDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GrainDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GrainDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void GrainDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GrainDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec mDelaySpec = { sampleRate/samplesPerBlock,samplesPerBlock,2 };
    mDelayLine = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>(MAXDELAYTIME*sampleRate);
    mDelayLine.prepare(mDelaySpec);
    mDelayLine2 = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>(MAXDELAYTIME * sampleRate);
    mDelayLine2.prepare(mDelaySpec);
    
}

void GrainDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GrainDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
//essentially a windowing function, this updates the counter within the function
inline float GrainDelayAudioProcessor::getGrainAmplitude(int &counter)
{
    //TODO: design better windowing functions that prevent audible clicks and pops
    counter = (counter + 1) %grainSize;
    //triangular
    float retval = 1.0f - ((2.0f / grainSize) * (abs(counter - 0.5f * grainSize)));
    
    //welch
    //float retval = 0.5f * (1.0f + std::cos(2 * juce::MathConstants<float>::twoPi * (counter - 0.5 * grainSize) / grainSize));
    return retval;    

}

void GrainDelayAudioProcessor::printGrainCounter() {
    for (int i = 0; i < 2; ++i)
    {
        DBG(grainCounter[i]);
    }
}
void GrainDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //////////////////////////////// MY CODE //////////////////////////////////////

    //retrieve parameters 
    int bufferSize = buffer.getNumSamples();
    int bufferRemaining = buffer.getNumSamples() - grainSamplesRemaining; //this is the remaining space of the grain samples    
    float newguidelaytime = mParamTree.getRawParameterValue("DELAYTIMEID")->load();    
    float drywet = mParamTree.getRawParameterValue("DRYWETID")->load();
    float rate = mParamTree.getRawParameterValue("FREQUENCYID")->load();    
    float width = mParamTree.getRawParameterValue("WIDTHID")->load();
    float spray = mParamTree.getRawParameterValue("SPRAYID")->load();    
        
    //update delaytime if changed
    if (newguidelaytime != guidelaytime)
    {
        guidelaytime = newguidelaytime;
        mDelayLine.setDelay(guidelaytime);
    } 

            
    //finishing adding the reset of the grain to the buffer
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* bufferptr = buffer.getWritePointer(channel);            
        for (int sample = 0; sample <std::min(grainSamplesRemaining,bufferSize); ++sample)
        {
            mDelayLine.pushSample(channel, bufferptr[sample]);          
            float wetsignal = getGrainAmplitude(grainCounter[channel]) * mDelayLine.popSample(channel);
            mDelayLine2.pushSample(channel, wetsignal);
            bufferptr[sample] =(bufferptr[sample] * (1.0f - drywet)) + ((drywet * ((0.5f-(channel-0.5f)*width)*wetsignal))+ (((0.5f + (channel - 0.5f) * width)* mDelayLine2.popSample(channel))));
        }
    }      
        
    //add new grains to the buffer
    for (int batch = grainSamplesRemaining; batch < bufferSize ; batch += grainSize)            
    {
        grainSize = (int)2 * getSampleRate() * (1.0f / rate); // in samples        
        // NEW GRAIN stats from here //
        randDelay = (guidelaytime + (spray * rng.nextFloat())) * 0.001f * getSampleRate();
       
        mDelayLine.setDelay(randDelay);        
        //the following lines ensure the side grains are not just simple delays of each other 
        int oddNum = (2*(abs(rng.nextInt())%5))+1;
        mDelayLine2.setDelay(grainSize*0.5*oddNum);
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            auto* bufferptr = buffer.getWritePointer(channel);
            for (int sample = batch; sample < std::min(batch+grainSize, bufferSize); ++sample)
            {                    
                mDelayLine.pushSample(channel, bufferptr[sample]);
                float wetsignal = getGrainAmplitude(grainCounter[channel]) * mDelayLine.popSample(channel);
                mDelayLine2.pushSample(channel, wetsignal);
                bufferptr[sample] = (bufferptr[sample] * (1.0f - drywet)) + ((drywet * ((0.5f - (channel - 0.5f) * width) * wetsignal)) + (((0.5f + (channel - 0.5f) * width) * mDelayLine2.popSample(channel))));
            }
        }
    }      
    
    //update variable for the next block 
    if (grainSamplesRemaining < bufferSize) grainSamplesRemaining = grainSize - (bufferRemaining % grainSize);
    else grainSamplesRemaining -= bufferSize;
}

//==============================================================================
bool GrainDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GrainDelayAudioProcessor::createEditor()
{
    return new GrainDelayAudioProcessorEditor (*this);
}

//==============================================================================
void GrainDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GrainDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GrainDelayAudioProcessor();
}


juce::AudioProcessorValueTreeState::ParameterLayout GrainDelayAudioProcessor::createParameterLayout()
{

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    //in Hzs
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("SPRAYID", "SPRAY", 0.0F, 1000.0f, 0.0f));//ms

    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("FREQUENCYID", "FREQUENY", 1.0f, 150.50f, 30.0f));//Hz

    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("DRYWETID", "DRY/WET", 0.0f, 1.0f, 0.5f));

    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("FEEDBACKID", "FEEDBACK", 0.0f, 0.950f, 0.0f));
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("DELAYTIMEID", "DELAYTIME", 0.1f, 2000.0f, 1000.0f));
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("WIDTHID", "WIDTH", 0.0f, 1.0f, 0.0f));



    return { params.begin(),params.end() };

}