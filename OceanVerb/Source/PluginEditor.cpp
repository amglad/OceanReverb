/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OceanVerbAudioProcessorEditor::OceanVerbAudioProcessorEditor (OceanVerbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    // hpf Knob
    hpfKnob.addListener(this);
    hpfKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    hpfKnob.setBounds(20,120,120,120);
    hpfKnob.setRange(20,2000,1);
    hpfKnob.setSkewFactorFromMidPoint(800);
    hpfKnob.setValue(500);
    hpfKnob.setTextValueSuffix(" Hz");
    hpfKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    hpfKnob.setColour(juce::Slider::textBoxTextColourId, juce::Colour(221,219,212));
    hpfKnob.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour(62, 134, 157));
    hpfKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(221,219,212));
    hpfKnob.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(210,208,202));
    hpfKnob.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(133,176,194));
    addAndMakeVisible(hpfKnob);
    
    hpfLabel.setText("hpf", juce::dontSendNotification);
    hpfLabel.attachToComponent(&hpfKnob, false);
    hpfLabel.setJustificationType(juce::Justification::centredTop);
    hpfLabel.setColour(juce::Label::textColourId, juce::Colour(221,219,212));
    hpfLabel.setFont(juce::Font("Futura", 18.0, juce::Font::plain));
    addAndMakeVisible(hpfLabel);
    
    
    // mix Knob
    mixKnob.addListener(this);
    mixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixKnob.setBounds(140,150,120,120);
    mixKnob.setRange(0,100,0.1);
    mixKnob.setValue(50);
    mixKnob.setTextValueSuffix(" %");
    mixKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    mixKnob.setColour(juce::Slider::textBoxTextColourId, juce::Colour(221,219,212));
    mixKnob.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour(62, 134, 157));
    mixKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(221,219,212));
    mixKnob.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(210,208,202));
    mixKnob.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(133,176,194));
    addAndMakeVisible(mixKnob);
    
    mixLabel.setText("mix", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixKnob, false);
    mixLabel.setJustificationType(juce::Justification::centredTop);
    mixLabel.setColour(juce::Label::textColourId, juce::Colour(221,219,212));
    mixLabel.setFont(juce::Font("Futura", 18.0, juce::Font::plain));
    addAndMakeVisible(mixLabel);
    
    
    // lpf Knob
    lpfKnob.addListener(this);
    lpfKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lpfKnob.setBounds(260,120,120,120);
    lpfKnob.setRange(800,20000,1);
    lpfKnob.setSkewFactorFromMidPoint(4000);
    lpfKnob.setValue(6000);
    lpfKnob.setTextValueSuffix(" Hz");
    lpfKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    lpfKnob.setColour(juce::Slider::textBoxTextColourId, juce::Colour(221,219,212));
    lpfKnob.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour(62, 134, 157));
    lpfKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(133,176,194));
    lpfKnob.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(210,208,202));
    lpfKnob.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(221,219,212));
    addAndMakeVisible(lpfKnob);
    
    lpfLabel.setText("lpf", juce::dontSendNotification);
    lpfLabel.attachToComponent(&lpfKnob, false);
    lpfLabel.setJustificationType(juce::Justification::centredTop);
    lpfLabel.setColour(juce::Label::textColourId, juce::Colour(221,219,212));
    lpfLabel.setFont(juce::Font("Futura", 18.0, juce::Font::plain));
    addAndMakeVisible(lpfLabel);
    
    // IR box
//    selectIR.addListener(this);
//    selectIR.setBounds(20, 20, 200, 30);
//    selectIR.addItem("6 feet back, 6 feet up", 1);
//    selectIR.addItem("12 feet back, 6 feet up", 2);
//    selectIR.addItem("18 feet back, 6 feet up", 3);
//    selectIR.addItem("18 feet back, 12 feet up", 4);
//    selectIR.addItem("18 feet back (side), 6 feet up", 5);
//    selectIR.addItem("18 feet back (side), 12 feet up", 6);
//    selectIR.addItem("18 feet back (side), 18 feet up", 7);
//    addAndMakeVisible(selectIR);
    
    // bypass button
    bypass.addListener(this);
    bypass.setBounds(320,10,80,20);
    bypass.setButtonText("bypass");
    bypass.setColour(juce::ToggleButton::textColourId, juce::Colour(221,219,212));
    bypass.setColour(juce::ToggleButton::tickColourId, juce::Colour(210,208,202));
    bypass.setToggleState(audioProcessor.isBypassed, juce::dontSendNotification);
    // addAndMakeVisible(bypass);

}

OceanVerbAudioProcessorEditor::~OceanVerbAudioProcessorEditor()
{
}

//==============================================================================
void OceanVerbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colour(62, 134, 157));
    
    waves = juce::ImageCache::getFromMemory(BinaryData::waves_png, BinaryData::waves_pngSize);
    waves.rescaled(70,70, juce::Graphics::highResamplingQuality);
    g.setColour(juce::Colour(210,208,202));
    int wavesWidth = waves.getWidth();
    int wavesHeight = waves.getHeight();
    g.drawImage (waves, 165, 30, 70, 70, 0, 0, wavesWidth, wavesHeight, true);

}

void OceanVerbAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void OceanVerbAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &lpfKnob)
    {
        audioProcessor.lpfValue = slider->getValue();
    }
    if (slider == &hpfKnob)
    {
        audioProcessor.hpfValue = slider->getValue();
    }
    if (slider == &mixKnob)
    {
        audioProcessor.mixValue = slider->getValue();
    }
}

void OceanVerbAudioProcessorEditor::buttonClicked(juce::Button *button){
    if (button == &bypass){
        // change value in PluginProcessor
        audioProcessor.isBypassed = bypass.getToggleState();
    }
}

void OceanVerbAudioProcessorEditor::comboBoxChanged(juce::ComboBox *comboBox)
{
    
    if (comboBox == &selectIR){
        if (selectIR.getSelectedId() == 1){
            // Do something for whole note
        }
        if (selectIR.getSelectedId() == 2){
            // Do something for half note
        }
        if (selectIR.getSelectedId() == 3){
            // Do something for quarter note
        }
        if (selectIR.getSelectedId() == 4){
            // Do something for 8th note
        }
        if (selectIR.getSelectedId() == 5){
            // Do something for 8th note
        }
        if (selectIR.getSelectedId() == 6){
            // Do something for 8th note
        }
        if (selectIR.getSelectedId() == 7){
            // Do something for 8th note
        }
    }
    
}
