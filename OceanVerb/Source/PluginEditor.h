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
class OceanVerbAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        public juce::Slider::Listener,
                                        public juce::Button::Listener,
                                        public juce::ComboBox::Listener
{
public:
    OceanVerbAudioProcessorEditor (OceanVerbAudioProcessor&);
    ~OceanVerbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    virtual void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked (juce::Button * button) override;
    void comboBoxChanged (juce::ComboBox *comboBox) override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OceanVerbAudioProcessor& audioProcessor;
    
    juce::Slider lpfKnob;
    juce::Slider hpfKnob;
    juce::Slider mixKnob;
    juce::ComboBox selectIR;
    juce::ToggleButton bypass;
    
    juce::Label lpfLabel;
    juce::Label hpfLabel;
    juce::Label mixLabel;
    juce::Label selectLabel;
    
    juce::LookAndFeel_V4 lookAndFeelV4;
    
    juce::Image waves;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OceanVerbAudioProcessorEditor)
};
