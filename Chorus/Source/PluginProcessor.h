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
class ChorusAudioProcessor : public juce::AudioProcessor
                            
{
public:
    //==============================================================================
    ChorusAudioProcessor();
    ~ChorusAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    

    juce::AudioProcessorValueTreeState mParamTree;
    float  offsetToEditor1;
    float offsetToEditor2;
    float getDryWet() { return mParamTree.getRawParameterValue("DRYWETID")->load(); }
private:


    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    float mSampleRate;
    static const int numDelayLines = 2;
    float delayTimes[10] = { 0.02f,0.01f, 0.007f,0.008f,0.009f,0.03f,0.025f };
    float panValues[4] = { 0.35f, 0.65f,0.8f,0.2f};
    //delay line attributes
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> mDelayLines[numDelayLines];
    float MAXDELAY = 20.0f; // in milliseconds

    const float mHighpashCutoff = 1000.0f;
    juce::dsp::IIR::Filter<float> mHighPassFilter[2];
    

    juce::dsp::Oscillator<float> lfosleft[numDelayLines];    
    juce::dsp::Oscillator<float> lfosright[numDelayLines];


        //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusAudioProcessor)
};
