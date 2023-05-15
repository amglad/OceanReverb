/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Biquads/BiquadLPF.h"
#include "Biquads/BiquadHPF.h"

//==============================================================================
/**
*/
class OceanVerbAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    OceanVerbAudioProcessor();
    ~OceanVerbAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    bool isBypassed = false;
    
    
    void setLPF(float lpfValue);
    void setHPF(float hpfValue);
    
    float lpfValue = 500.f;
    float hpfValue = 6000.f;
    
    float mixValue = 50;
    float mix;
    float smoothMix[2] = {0.f};
    float alpha = 0.999f;
    void setMix(float mixValue);
    
    
    double IRFS = 96000;
    int irDataSize = 1333036;
    int irNumSamples = 166624;
    juce::AudioBuffer<float> IR {2, irNumSamples};
    
    juce::dsp::Convolution conv;
    juce::dsp::ProcessSpec spec;
    
    juce::HeapBlock<char> impulseResponseData;
    juce::File impulseResponseFile;
    
private:
    
    BiquadLPF lpFilter {BiquadLPF::FilterType::LPF, 0.7071f};
    BiquadHPF hpFilter {BiquadHPF::FilterType::HPF, 0.7071f};
   
    int count = 0;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OceanVerbAudioProcessor)
};
