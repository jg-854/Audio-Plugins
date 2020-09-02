/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#define WIDTH 800
#define HEIGHT 600
#define HALFWIDTH 400



//==============================================================================
RealTimeGranulatorAudioProcessorEditor::RealTimeGranulatorAudioProcessorEditor (RealTimeGranulatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    getLookAndFeel().setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::white);
    getLookAndFeel().setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(61, 112, 115));
    getLookAndFeel().setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::transparentBlack);
    getLookAndFeel().setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colour(61, 112, 115));
    getLookAndFeel().setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::white);


    

    
    frequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "FREQUENCYID", frequencySlider);
    frequencyLabel.setText("Frequency", juce::NotificationType::dontSendNotification);
    frequencyLabel.attachToComponent(&frequencySlider, false);
    frequencyLabel.setJustificationType(juce::Justification::centred);
    frequencySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    frequencySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(frequencySlider);
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
    sizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "SIZEID", sizeSlider);
    sizeLabel.setText("Size", juce::NotificationType::dontSendNotification);
    sizeLabel.attachToComponent(&sizeSlider, false);
    sizeLabel.setJustificationType(juce::Justification::centred);
    sizeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    sizeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(sizeSlider);
  
    dwAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParamTree, "DRYWETID", dwSlider);

    dwLabel.setText("Dry/Wet", juce::NotificationType::dontSendNotification);
    dwLabel.attachToComponent(&dwSlider, false);
    dwLabel.setJustificationType(juce::Justification::centred);

    dwSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(dwSlider);


    frequencySlider.setBounds(580, 40, 120, 120);
    widthSlider.setBounds(530, 180, 80, 80);
    sizeSlider.setBounds(670, 180, 80, 80);
    spraySlider.setBounds(580, 300, 120, 120);
    dwSlider.setBounds(510, 430, 260, 100);

    frequencySlider.setRange(1, 150, 0.1);
    widthSlider.setRange(0.0, 1.0, 0.01);
    sizeSlider.setRange(3.0, 200.0, 0.1);
    spraySlider.setRange(0.0, 1000.0, 1.0);
    dwSlider.setRange(0.0, 1.0, 0.01);

    setSize(800, 600);
    startTimerHz(25);
}

RealTimeGranulatorAudioProcessorEditor::~RealTimeGranulatorAudioProcessorEditor()
{
}

//==============================================================================
void RealTimeGranulatorAudioProcessorEditor::paint (juce::Graphics& g)
{

    float editorDryWet = audioProcessor.mParamTree.getRawParameterValue("DRYWETID")->load();
    
    int editorSize =  audioProcessor.mParamTree.getRawParameterValue("SIZEID")->load();
   

    
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(jeevColoursBackground);
    g.setColour(jeevColoursMiddle);
    g.drawRoundedRectangle(HALFWIDTH + 100, HEIGHT / 30, HALFWIDTH - 120, 0, 50.0f, 2.0f);
    g.drawRoundedRectangle(HALFWIDTH + 100, HEIGHT * 0.7, 100, 0, 50.0f, 2.0f);
    g.drawRoundedRectangle(HALFWIDTH + 280, HEIGHT * 0.7, 95,0, 50.0f, 2.0f);
    g.drawRoundedRectangle(HALFWIDTH + 100, HEIGHT * 0.9, HALFWIDTH - 120, 0, 50.0f, 2.0f);   
    g.fillRoundedRectangle(50, 500, 300, 50, 10.0f);

    for (int i = 0; i < grainvec.size(); i++) {
        g.setColour(juce::Colour(juce::uint8(244), juce::uint8(244),juce::uint8(244),grainvec[i].counter / 500.0f));

        g.fillEllipse((1.0f- (grainvec[i].counter/500.0f))*grainvec[i].x_coord+200, grainvec[i].counter, 10.0, 10.0f);

    }
    

}

void RealTimeGranulatorAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void RealTimeGranulatorAudioProcessorEditor::timerCallback() {
    float freq = audioProcessor.mParamTree.getRawParameterValue("FREQUENCYID")->load();
    int editorFreqLength = (int)(25.0f / pow(1.02f ,(freq - 1)));
    float editorSprayRatio = audioProcessor.mParamTree.getRawParameterValue("SPRAYID")->load() / 1000.0f;
    int editorSize = (int)(200.0/log(audioProcessor.mParamTree.getRawParameterValue("SIZEID")->load()));

    if (y++ % editorFreqLength == 0)grainvec.push_back({ 500,(int)(300.0f * (rng.nextFloat() - 0.5f) * editorSprayRatio)});
       
    for (int i = 0; i < grainvec.size(); i++) {
        grainvec[i].counter -= editorSize;
        if (grainvec[i].counter < -20) grainvec.erase(grainvec.begin() + i);
    }
    repaint();

}