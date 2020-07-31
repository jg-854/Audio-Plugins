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



struct editorGrain {
    int counter;
    int x_coord;

};
class RealTimeGranulatorAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                public juce::Timer
{
public:
    RealTimeGranulatorAudioProcessorEditor (RealTimeGranulatorAudioProcessor&);
    ~RealTimeGranulatorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
private:

   
    std::vector<editorGrain> grainvec;
    juce::Random rng;
    int y{ 0 };
    
    juce::Slider frequencySlider;
    juce::Slider spraySlider;
    juce::Slider widthSlider;
    juce::Slider sizeSlider;
    juce::Label frequencyLabel;
    juce::Label sprayLabel;
    juce::Label widthLabel;
    juce::Label sizeLabel;
    juce::Slider dwSlider;
    juce::Label dwLabel;

    juce::Colour jeevColoursBackground{ 31, 40, 46 };
    juce::Colour jeevColoursMiddle{ 91, 120, 138 };
    juce::Colour jeevColoursCool{ 150, 190, 217 };



    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sizeAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dwAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sprayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RealTimeGranulatorAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RealTimeGranulatorAudioProcessorEditor)
};
