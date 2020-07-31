/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RealTimeGranulatorAudioProcessor::RealTimeGranulatorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    mParamTree(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

RealTimeGranulatorAudioProcessor::~RealTimeGranulatorAudioProcessor()
{
}

//==============================================================================
const juce::String RealTimeGranulatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RealTimeGranulatorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RealTimeGranulatorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RealTimeGranulatorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RealTimeGranulatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RealTimeGranulatorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RealTimeGranulatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RealTimeGranulatorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RealTimeGranulatorAudioProcessor::getProgramName (int index)
{
    return {};
}

void RealTimeGranulatorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RealTimeGranulatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec mDelaySpec = { sampleRate,samplesPerBlock,2 };
    mDelayLine = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>(MAXDELAYTIME * sampleRate);
    mDelayLine.prepare(mDelaySpec);

}

void RealTimeGranulatorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RealTimeGranulatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void RealTimeGranulatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
    ///////////////// MY CODE /////////////////


    float delayInMillis = mParamTree.getRawParameterValue("DELAYTIMEID")->load();
    float newSizeInMillis = mParamTree.getRawParameterValue("SIZEID")->load();
    float wet = mParamTree.getRawParameterValue("DRYWETID")->load();
    float newFrequency = mParamTree.getRawParameterValue("FREQUENCYID")->load();
    float spray = mParamTree.getRawParameterValue("SPRAYID")->load();
    float width = mParamTree.getRawParameterValue("WIDTHID")->load();

    int delayInSamples = (int)delayInMillis * getSampleRate() * 0.001;

    if (mFrequency != newFrequency) {
        mFrequency = newFrequency;
        waitcounter[0] = 0;
        waitcounter[1] = 0;
        //endGrain[0] = 0;
        //endGrain[1] = 0;
        //startGrain[0] = 0;
        //startGrain[1]=0;
        waittime = (int)getSampleRate() / mFrequency;
    }

    /*the startGrain and endGrain arrays need to be reset as if we suddnely change the size parameter to something really small,
    the assumption of the queue structure (the endGrain index is the grain that will finish first), is incorrect. */
    if (mSizeInMillis != newSizeInMillis) {
        mSizeInMillis = newSizeInMillis;
        sizeInSamples = (int)mSizeInMillis * getSampleRate() * 0.001;
        endGrain[0] = 0;
        endGrain[1] = 0;
        startGrain[0] = 0;
        startGrain[1] = 0;
    }


    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto bufferptr = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float dryval = bufferptr[sample];
            // code to determine whether a new grain needs ot be created
            if (waitcounter[channel] == 0) {
                if (channel == 0) grainarr[endGrain[channel]] = grain(sizeInSamples, (delayInMillis + (spray * rng.nextFloat())) * 0.001 * getSampleRate());
                endGrain[channel] = (endGrain[channel] + 1) % MAXNUMBERGRAINS;
            }
            waitcounter[channel] = (waitcounter[channel] + 1) % waittime;


            //iterating through the queue of grains and retrieving the sample from the delay buffer
            float wetval = 0.0f;
            int gidx = startGrain[channel];
            while (gidx != endGrain[channel]) {
                if (grainarr[gidx].endOfGrain(channel) == true) startGrain[channel] = (startGrain[channel] + 1) % MAXNUMBERGRAINS;

                else wetval += grainarr[gidx].getGrainAmplitude(channel) * mDelayLine.popSample(channel, grainarr[gidx].getDelayInSamples(), false);
                gidx = (gidx + 1) % MAXNUMBERGRAINS;
            }
            //just to update the read pointer
            mDelayLine.popSample(channel, 23, true);
            bufferptr[sample] = (dryval * (1.0f - wet)) + wetval * wet;
            mDelayLine.pushSample(channel, dryval);
        }
    }
    editorStartGrain = startGrain[0];
    editorEndGrain = endGrain[0];



    


}

//==============================================================================
bool RealTimeGranulatorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RealTimeGranulatorAudioProcessor::createEditor()
{
    return new RealTimeGranulatorAudioProcessorEditor (*this);
}

//==============================================================================
void RealTimeGranulatorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RealTimeGranulatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RealTimeGranulatorAudioProcessor();
}


juce::AudioProcessorValueTreeState::ParameterLayout RealTimeGranulatorAudioProcessor::createParameterLayout()
{

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    //in Hzs
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("SPRAYID", "SPRAY", 0.0F, 1000.0f, 250.0f));//ms
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("FREQUENCYID", "FREQUENY", 1.0f, 150.50f, 10.0f));//Hz
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("DRYWETID", "DRY/WET", 0.0f, 1.0f, 1.0f));
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("DELAYTIMEID", "DELAYTIME", 0.1f, 2000.0f, 0.1f));
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("WIDTHID", "WIDTH", 0.0f, 1.0f, 0.0f));
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>("SIZEID", "SIZE", 3.0f, 200.0f, 200.0f)); //in millis

    return { params.begin(),params.end() };

}