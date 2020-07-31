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
class ChorusAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        public juce::Timer
{
public:
    ChorusAudioProcessorEditor (ChorusAudioProcessor&);
    ~ChorusAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;


private:
    void timerCallback() override;
    float width1{ 0 };
    float width2{ 0 };
    
    juce::Slider lfoRateSlider;
    juce::Slider lfoDepthSlider;
    juce::Slider dryWetSlider;
    juce::Slider feedBackSlider;


    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;

    juce::Label lfoRateLabel;
    juce::Label lfoDepthLabel;
    juce::Label feedbackLabel;
    juce::Label dryWetLabel;









    ChorusAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusAudioProcessorEditor)
};
