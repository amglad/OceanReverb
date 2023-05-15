/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OceanVerbAudioProcessor::OceanVerbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

OceanVerbAudioProcessor::~OceanVerbAudioProcessor()
{
}

//==============================================================================
const juce::String OceanVerbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OceanVerbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OceanVerbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OceanVerbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OceanVerbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OceanVerbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OceanVerbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OceanVerbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OceanVerbAudioProcessor::getProgramName (int index)
{
    return {};
}

void OceanVerbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OceanVerbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    lpFilter.setFs(sampleRate);
    hpFilter.setFs(sampleRate);
    
    // Setting up specifications
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    // Load impulse response file
    juce::File impulseResponse("/Users/mitchglad/Sound Library/Sound Library/Impulse Responses/Ocean Way Studios/96k, 32-bit float/Side angle, 12 feet up-96k.wav");
    
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
   
    std::unique_ptr<juce::AudioFormatReader> audioReader(formatManager.createReaderFor(impulseResponse));
        
    // Check if the reader was created successfully
    if (audioReader == nullptr) {
        std::cout << "Failed to load WAV file" << std::endl;
        return;
    }
    
    // Read the WAV file into the AudioBuffer
    audioReader->read(&IR, 0, irNumSamples, 0, true, true);

    conv.loadImpulseResponse(juce::AudioBuffer<float>(IR),
                              IRFS,
                              juce::dsp::Convolution::Stereo::yes,
                              juce::dsp::Convolution::Trim::no,
                              juce::dsp::Convolution::Normalise::yes);
    
    conv.prepare(spec);
    conv.reset();
}

    

void OceanVerbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OceanVerbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OceanVerbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    int numSamples = buffer.getNumSamples();
    juce::AudioBuffer<float> wetBuffer {totalNumOutputChannels, numSamples};
    
    for (int c = 0; c < totalNumOutputChannels; ++c)
    {
        for (int n = 0; n < numSamples; ++n)
        {
            wetBuffer.getWritePointer(c)[n] = buffer.getWritePointer(c)[n];
        }
    }

    juce::dsp::AudioBlock<float> block (wetBuffer);
    auto context = juce::dsp::ProcessContextReplacing<float> (block);
    
    conv.process(context);
    block.copyTo(wetBuffer);
    
    lpFilter.setFreq(lpfValue);
    hpFilter.setFreq(hpfValue);
    setMix(mixValue);
    
    for (int c = 0; c < totalNumOutputChannels; ++c)
    {
        for (int n = 0; n < numSamples; ++n)
        {
            lpFilter.setFreq(lpfValue);
            hpFilter.setFreq(hpfValue);
            
            float w = wetBuffer.getWritePointer(c)[n] * 5.f;
            w = lpFilter.processSample(w, c);
            w = hpFilter.processSample(w, c);
            
            wetBuffer.getWritePointer(c)[n] = w;
            smoothMix[c] = alpha * smoothMix[c] + (1.f - alpha) * mix;
            float y = (wetBuffer.getWritePointer(c)[n] * smoothMix[c]) + (buffer.getWritePointer(c)[n] * (1.f-smoothMix[c]));
            buffer.getWritePointer(c)[n] = y;
        }
    }
}


//==============================================================================
bool OceanVerbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OceanVerbAudioProcessor::createEditor()
{
    return new OceanVerbAudioProcessorEditor (*this);
}

//==============================================================================
void OceanVerbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("Params"));
     xml->setAttribute("LPF", (double) lpfValue);
     xml->setAttribute("HPF", (double) hpfValue);
     xml->setAttribute("Mix", (double) mix);
}

void OceanVerbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName ("Params")) {
            lpfValue = xmlState->getDoubleAttribute ("LPF", 500.0);
            hpfValue = xmlState->getDoubleAttribute ("HPF", 6000.0);
            mix = xmlState->getDoubleAttribute ("Mix", 50.0);
        }
}

void OceanVerbAudioProcessor::setMix(float mixValue)
{
    mix = mixValue/100.f;
}

void OceanVerbAudioProcessor::setLPF(float lpfValue)
{
    lpfValue = lpfValue;
}

void OceanVerbAudioProcessor::setHPF(float hpfValue)
{
    hpfValue = hpfValue;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OceanVerbAudioProcessor();
}
