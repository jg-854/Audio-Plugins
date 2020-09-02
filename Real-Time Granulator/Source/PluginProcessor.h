/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
class grain
{

private:
    int size;
    int delay;
    int counter[2]{ 0,0 };
public:
    grain() :size(0), delay(0) {

    };
    grain(int grainSizeInSamples, int delayInSamples) : size(grainSizeInSamples), delay(delayInSamples)
    {
    };


    int getDelayInSamples() {
        return delay;
    };
    float getGrainAmplitude(int channel)
    {
        return  1.0f - ((2.0f / size) * (abs((int)(counter[channel]++ - 0.5f * size))));
    };
    bool endOfGrain(int channel) {
        return size == counter[channel];
    };
    //returns a float showing how far a grain is through its course
    float getPercentage() {
        float retval = (counter[0] / size);
        return retval;
    }


    void reset(int grainSizeInSamples, int delayInSamples)
    {
        size = grainSizeInSamples;
        delay = delayInSamples;
        counter[0] = 0;
        counter[1] = 0;
    };


};
//==============================================================================
/**
*/
class RealTimeGranulatorAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    RealTimeGranulatorAudioProcessor();
    ~RealTimeGranulatorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    juce::AudioProcessorValueTreeState mParamTree;


    int editorStartGrain{ 0 };
    int editorEndGrain{ 0 };

    static const int MAXNUMBERGRAINS = 30;
private:
    //private functions 

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();


    // can be calculated by maximum frequency *  maximum grain time in seconds
    
    float MAXDELAYTIME = 2000.0f;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> mDelayLine;
    //this gives additive randomness to the base delay
    juce::Random rng;

    //grain grainarr[MAXNUMBERGRAINS];



    int waittime{ 0 };
    int waitcounter[2]{ 0,0 };
    int startGrain[2] = { 0,0 };
    int endGrain[2] = { 0,0 };
    float mFrequency{ 0.0f };
    int sizeInSamples{ 0 };
    float mSizeInMillis{ 0.0f };

    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RealTimeGranulatorAudioProcessor)


public:
    grain grainarr[MAXNUMBERGRAINS];


     

};


