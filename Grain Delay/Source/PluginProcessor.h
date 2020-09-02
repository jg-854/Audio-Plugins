/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class GrainDelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GrainDelayAudioProcessor();
    ~GrainDelayAudioProcessor() override;

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

private:
    //private functions 

    inline float getGrainAmplitude(int &counter);
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    
    //delay attributes
    float MAXDELAYTIME = 2000.0f;
    juce::dsp::DelayLine<float,juce::dsp::DelayLineInterpolationTypes::Linear> mDelayLine;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>mDelayLine2;
    juce::Random rng;
    //the delay that the user has selected from the gui
    float guidelaytime{ 0.0f };
    //the current delay of the grain which is randomly generated from the guidelayitme
    float randDelay{ 0.0f };
    //this size depends on the frequency of grains emitted
    int grainSize{ 0 };
    int grainSamplesRemaining{ 0 };
    //this keeps track of where the sample position is within a grain to control the ampltidue (windowing function)
    int grainCounter[2] = { 0,0 };
    
    //debugging function
    void printGrainCounter();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainDelayAudioProcessor)
};
