/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
GrainDelayAudioProcessorEditor::GrainDelayAudioProcessorEditor (GrainDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    getLookAndFeel().setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
    getLookAndFeel().setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(61, 112, 115));
    getLookAndFeel().setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::transparentBlack);
    getLookAndFeel().setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colour(61, 112, 115));
    getLookAndFeel().setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::white);



    delaytimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "DELAYTIMEID", delaytimeSlider);
    delaytimeLabel.setText("Base Delay", juce::NotificationType::dontSendNotification);
    delaytimeLabel.attachToComponent(&delaytimeSlider, false);
    delaytimeLabel.setJustificationType(juce::Justification::centred);
    delaytimeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    delaytimeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    delaytimeSlider.setBounds(0, 0, 0, 0);
    addAndMakeVisible(delaytimeSlider);
    sprayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "SPRAYID", spraySlider);
    sprayLabel.setText("Spray", juce::NotificationType::dontSendNotification);
    sprayLabel.attachToComponent(&spraySlider, false);
    sprayLabel.setJustificationType(juce::Justification::centred);
    spraySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    spraySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(spraySlider);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "WIDTHID", widthSlider);
    widthLabel.setText("Width", juce::NotificationType::dontSendNotification);
    widthLabel.attachToComponent(&widthSlider, false);
    widthLabel.setJustificationType(juce::Justification::centred);
    widthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    widthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(widthSlider);
    frequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "FREQUENCYID", frequencySlider);
    frequencyLabel.setText("Frequency", juce::NotificationType::dontSendNotification);
    frequencyLabel.attachToComponent(&frequencySlider, false);
    frequencyLabel.setJustificationType(juce::Justification::centred);
    frequencySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    frequencySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(frequencySlider);
    drywetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "DRYWETID", drywetSlider);
    drywetLabel.setText("Dry/Wet", juce::NotificationType::dontSendNotification);
    drywetLabel.attachToComponent(&drywetSlider, false);
    drywetLabel.setJustificationType(juce::Justification::centred);
    drywetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    drywetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(drywetSlider);

    frequencySlider.setBounds(580, 40, 120, 120);
    widthSlider.setBounds(580, 180, 120, 120);
   
    spraySlider.setBounds(580, 300, 120, 120);
    drywetSlider.setBounds(580, 430,120, 120);

    frequencySlider.setRange(1, 150, 0.1);
    widthSlider.setRange(0.0, 1.0, 0.01);
   
    spraySlider.setRange(0.0, 1000.0, 1.0);
    drywetSlider.setRange(0.0, 1.0, 0.01);
   
    setSize(800, 600);
}

GrainDelayAudioProcessorEditor::~GrainDelayAudioProcessorEditor()
{
}

//==============================================================================
void GrainDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

}

void GrainDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

