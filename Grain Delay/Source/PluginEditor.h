/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GrainDelayAudioProcessorEditor  : public juce::AudioProcessorEditor

                              
{
public:
    GrainDelayAudioProcessorEditor (GrainDelayAudioProcessor&);
    ~GrainDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

  

private:



    juce::Slider delaytimeSlider;
    juce::Slider spraySlider;
    juce::Slider widthSlider;
    juce::Slider frequencySlider;
    juce::Slider drywetSlider;
    juce::Label delaytimeLabel;
    juce::Label sprayLabel;
    juce::Label widthLabel;
    juce::Label frequencyLabel;
    juce::Label drywetLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>delaytimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>sprayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>widthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>frequencyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>drywetAttachment;
    

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GrainDelayAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainDelayAudioProcessorEditor)




    
};
