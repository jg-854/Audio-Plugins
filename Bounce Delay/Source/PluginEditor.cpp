/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BounceDelayAudioProcessorEditor::BounceDelayAudioProcessorEditor (BounceDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    getLookAndFeel().setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(224, 122, 95));
    getLookAndFeel().setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(235, 166, 148));
    getLookAndFeel().setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::transparentBlack);
    getLookAndFeel().setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colour(61, 112, 115));
    getLookAndFeel().setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::white);
    getLookAndFeel().setColour(juce::ToggleButton::tickColourId,juce::Colour(224, 122, 95));
    getLookAndFeel().setColour(juce::ToggleButton::tickDisabledColourId,juce::Colour(224, 122, 95));







    delayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "DELAYID", delaySlider);
    delayLabel.setText("Pre-Delay", juce::NotificationType::dontSendNotification);
    delayLabel.attachToComponent(&delaySlider, false);
    delayLabel.setJustificationType(juce::Justification::centred);
    delaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    delaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(delaySlider);
    drywetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "DRYWETID", drywetSlider);
    drywetLabel.setText("Dry/Wet", juce::NotificationType::dontSendNotification);
    drywetLabel.attachToComponent(&drywetSlider, false);
    drywetLabel.setJustificationType(juce::Justification::centred);
    drywetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    drywetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(drywetSlider);

    elasticityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "ELASTICITYID", elasticitySlider);
    elasticityLabel.setText("Ampltiude Elasticity", juce::NotificationType::dontSendNotification);
    elasticityLabel.attachToComponent(&elasticitySlider,false);
    elasticityLabel.setJustificationType(juce::Justification::centred);
    elasticitySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    elasticitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

    addAndMakeVisible(elasticitySlider);


    frequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "FREQUENCYID", frequencySlider);
    frequencyLabel.setText("Frequency Elasticity", juce::NotificationType::dontSendNotification);
    frequencyLabel.attachToComponent(&frequencySlider, false);
    frequencyLabel.setJustificationType(juce::Justification::centred);
    frequencySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    frequencySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(frequencySlider);

    elasticitySlider.setRange(0.3, 0.95, 0.01);  
    drywetSlider.setRange(0.0, 1.0, 0.001);

    delaySlider.setRange(1.0f, 500.0f, 1.0f);

    frequencySlider.setRange(0.8, 0.99, 0.01);


    delaySlider.setSize(100, 100);
    delaySlider.setCentrePosition(175, 500);
    
    frequencySlider.setSize(100, 100);
    frequencySlider.setCentrePosition(325, 500);

    elasticitySlider.setSize(100, 100);
    elasticitySlider.setCentrePosition(475, 500);

    drywetSlider.setSize(100, 100);
    drywetSlider.setCentrePosition(625, 500);

    

    stereoAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.mParamTree, "STEREOID", stereoToggle);
    reverseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.mParamTree, 
        "REVERSEID", reverseToggle);

    addAndMakeVisible(stereoToggle);
    stereoToggle.setBounds(705, 50, 50, 50);
    



    stereoLabel.setText("Stereo", juce::NotificationType::dontSendNotification);
    stereoLabel.attachToComponent(&stereoToggle, false);
    stereoLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(stereoToggle);


    

    reverseLabel.setText("Reverse", juce::NotificationType::dontSendNotification);
    reverseLabel.attachToComponent(&reverseToggle, false);
    reverseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverseToggle);
    reverseToggle.setBounds(75, 50,50,50);
    setSize (800, 600);


    startTimerHz(25);
}

BounceDelayAudioProcessorEditor::~BounceDelayAudioProcessorEditor()
{
}


//==============================================================================
void BounceDelayAudioProcessorEditor::paint (juce::Graphics& g)
{

    auto delay = audioProcessor.mParamTree.getRawParameterValue("DELAYID")->load() *0.5;
    float drywet = audioProcessor.mParamTree.getRawParameterValue("DRYWETID")->load();
    float elasticity = audioProcessor.mParamTree.getRawParameterValue("ELASTICITYID")->load();
    float warp = audioProcessor.mParamTree.getRawParameterValue("FREQUENCYID")->load();
    elasticity = std::sqrt(elasticity);
    warp = std::sqrt(warp);
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colour(129, 178, 154));
    g.setFont(15.0f);
    g.setColour (juce::Colour(60, 105, 83)); 
    g.fillRoundedRectangle(100.0f, 70.0f, 600.0f, 300.0f, 20.0f);

    g.setColour(juce::Colour(juce::uint8(129), 178, 154,drywet));
    const int  n_bounces = 50;
    int offset = 0;
    int multiplier = 1;
    if (reverseToggle.getToggleState()) {
        offset = 50;
        multiplier = -1;
    }

    const float start = 120.0f;
    const float height = 270.0f;
    for (int i = 0; i < 50; i+=2) {
        g.fillRect(start + delay + (i *(8+ ((delay / 20) +10*std::pow(warp,i)))),
            370.0f-(height * std::pow(elasticity, offset + (multiplier*i))), 5.0f, height * std::pow(elasticity, offset + (multiplier * i)));

    }
    if (stereoToggle.getToggleState())g.setColour(juce::Colour(juce::uint8(129), 178, 154, drywet*0.4f));
    for (int i = 1; i < 50; i += 2) {
        g.fillRect(start + delay + (i * (8 + ((delay / 20) + 10 * std::pow(warp, i)))),
            370.0f - (height * std::pow(elasticity, offset +(multiplier*i))), 5.0f, height * std::pow(elasticity, offset + (multiplier * i)));

    }
    
    

    



   

}

void BounceDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}


void BounceDelayAudioProcessorEditor::timerCallback() {
    this->repaint();
}
