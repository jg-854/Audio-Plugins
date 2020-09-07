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
class BounceDelayAudioProcessorEditor  : public juce::AudioProcessorEditor,
    public juce::Timer
                    
{
public:
    BounceDelayAudioProcessorEditor (BounceDelayAudioProcessor&);
    ~BounceDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

protected:
    void timerCallback() override;

private:

    juce::Slider delaySlider;
    juce::Slider drywetSlider;

    juce::Slider numberbouncesSlider;
    juce::Slider elasticitySlider;
    juce::Label delayLabel;
    juce::Label drywetLabel;

    juce::Label numberbouncesLabel;
    juce::Label elasticityLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>delayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>drywetAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>numberbouncesAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>elasticityAttachment;
  
    juce::Slider frequencySlider;
    juce::Label frequencyLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>frequencyAttachment;


    juce::ToggleButton stereoToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> stereoAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> reverseAttachment;

    juce::ToggleButton reverseToggle;
    juce::Label reverseLabel;
    juce::Label stereoLabel;

    BounceDelayAudioProcessor& audioProcessor;


    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BounceDelayAudioProcessorEditor)
};
