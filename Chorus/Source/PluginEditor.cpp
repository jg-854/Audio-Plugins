/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#define WIDTH 800
#define HEIGHT 600
//==============================================================================
ChorusAudioProcessorEditor::ChorusAudioProcessorEditor (ChorusAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    getLookAndFeel().setColour(juce::Slider::backgroundColourId, juce::Colour(101, 143, 103));
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colour(199, 255, 243));
    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colour(58, 214, 176));
    getLookAndFeel().setColour(juce::Slider::textBoxTextColourId, juce::Colours::beige);
    getLookAndFeel().setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::black);


    lfoRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "LFORATEID", lfoRateSlider);
    lfoRateSlider.setBounds(WIDTH/2, HEIGHT/4, 60, HEIGHT/2);
    lfoRateSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    lfoRateSlider.setRange(0.03, 10.0, 0.01);
    lfoRateSlider.setSkewFactorFromMidPoint(1.0);
    addAndMakeVisible(lfoRateSlider);
    lfoRateLabel.attachToComponent(&lfoRateSlider,false);
    lfoRateLabel.setText("Rate", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(lfoRateLabel);

    lfoDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "LFODEPTHID", lfoDepthSlider);
    lfoDepthSlider.setBounds(WIDTH/2 + 100, HEIGHT/4, 60, HEIGHT/2);
    lfoDepthSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    lfoDepthSlider.setRange(0.0, 6.5, 0.01);
    addAndMakeVisible(lfoDepthSlider);
    lfoDepthLabel.attachToComponent(&lfoDepthSlider, false);
    lfoDepthLabel.setText("Depth", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(lfoDepthLabel);


    feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "FEEDBACKID", feedBackSlider);
    feedBackSlider.setBounds(WIDTH / 2 + 200, HEIGHT / 4, 60, HEIGHT / 2);
    feedBackSlider.setRange(0.0, 1.0, 0.01);
    feedBackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    addAndMakeVisible(feedBackSlider);
    feedbackLabel.attachToComponent(&feedBackSlider, false);
    feedbackLabel.setText("Feedback", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(feedbackLabel);




    dryWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "DRYWETID", dryWetSlider);
    dryWetSlider.setBounds(WIDTH / 2 + 300, HEIGHT / 4, 60, HEIGHT / 2);
    dryWetSlider.setRange(0.0, 1.0, 0.01);
    dryWetSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    addAndMakeVisible(dryWetSlider);
    dryWetLabel.attachToComponent(&dryWetSlider, false);
    dryWetLabel.setText("Dry/Wet", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(dryWetLabel);



    setSize (800, 600);
    startTimerHz(25);
}

ChorusAudioProcessorEditor::~ChorusAudioProcessorEditor()
{
}

void ChorusAudioProcessorEditor::timerCallback() {


    width1 = 100.0f + audioProcessor.offsetToEditor1 * 0.1;
    width2 = 80.0f + audioProcessor.offsetToEditor2 * 0.1;

    repaint();



}

//==============================================================================
void ChorusAudioProcessorEditor::paint (juce::Graphics& g)
{
    float drywet = audioProcessor.mParamTree.getRawParameterValue("DRYWETID")->load();
    float feedback = audioProcessor.mParamTree.getRawParameterValue("FEEDBACKID")->load();

    g.fillAll(juce::Colour(47, 125, 106));

    
    g.setColour(juce::Colour(0.42f, 0.47f, 1.0f,drywet));
    
    float alpha = drywet;
    float radius = width1;
    for (int i = 0; i < 20; ++i) {

        g.setColour(juce::Colour(0.42f, 0.47f, 1.0f, alpha));
        g.drawEllipse(200 - radius, 300 - radius, 2 * radius, 2 * radius, 2.0f);                
        alpha *= 1.5*feedback;
        radius *= 0.92;            

    } 
    
  

    alpha = drywet;
    radius = width2;
    for (int i = 0; i < 20; ++i) {
        g.setColour(juce::Colour(0.64f, 0.150f, 1.0f, alpha));
        g.drawEllipse(200 - radius, 300 - radius, 2 * radius, 2 * radius, 2.0f);
        alpha *= 1.5*feedback;
        radius *= 0.92;
    }


    g.setFont (15.0f);
    

}

