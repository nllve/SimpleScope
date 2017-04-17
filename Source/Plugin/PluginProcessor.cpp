/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"



//==============================================================================
SmallScopeAudioProcessor::SmallScopeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(), oscilloscopeWaveform(), parameters (*this, nullptr)

#endif
{
    #if ! JucePlugin_IsMidiEffect
    #if ! JucePlugin_IsSynth
        BusesProperties().withInput  ("Input",  AudioChannelSet::stereo(), true);
    #endif
        BusesProperties().withOutput ("Output", AudioChannelSet::stereo(), true);
    #endif
    
    //PARAMETER LIST
    parameters.createAndAddParameter ("triggerMode",
                                      "TriggerMode",
                                      String(),
                                      NormalisableRange<float> (0.1f, 1.0f), 0.15f,
                                      [](float value)
                                      {
                                          if (value >= 0.1f && value < 0.333f)
                                              return "Off";
                                          else if (value >= 0.333f && value < 0.666f)
                                              return "Up";
                                          else
                                              return "Down";
                                      },
                                      [](const String& text)
                                      {
                                          if (text == "Off")    return 0.15f;
                                          else if (text == "Up")  return 0.45f;
                                          else return 0.75f;
                                      });
    
    
    parameters.createAndAddParameter ("verticalZoom",
                                      "VerticalZoom",
                                      String(),
                                      NormalisableRange<float> (1.0f, 2.0f), 1.0f,
                                      nullptr, nullptr);
    
    
    parameters.createAndAddParameter ("samplesPerPixel",                            // parameter ID
                                          "SamplesPerPixel",                            // parameter name
                                          String(),                                     // parameter label (suffix)
                                          NormalisableRange<float> (2.0f, 256.0f),      // range
                                          2.0f,                                         // default value
                                          nullptr,
                                          nullptr);
    
    
    parameters.createAndAddParameter ("phosphorDecay",                              // parameter ID
                                          "PhosphorDecay",                              // parameter name
                                          String(),                                     // parameter label (suffix)
                                          NormalisableRange<float> (0.005f, 0.3f),      // range
                                          0.0005f,                                       // default value
                                          nullptr,
                                          nullptr);
    
    parameters.createAndAddParameter ("waveformFill",                               // parameter ID
                                          "WaveformFill",                               // parameter name
                                          String(),                                     // parameter label (suffix)
                                          NormalisableRange<float> (0.0f, 0.028f),       // range
                                          0.0f,                                         // default value
                                          nullptr,
                                          nullptr);
    
    parameters.createAndAddParameter ("traceHue",                                   // parameter ID
                                          "TraceHue",                                   // parameter name
                                          String(),                                     // parameter label (suffix)
                                          NormalisableRange<float> (0.22f, 1.22f),      // range
                                          0.22f,                                        // default value
                                          nullptr,
                                          nullptr);
    
    parameters.state = ValueTree (Identifier ("SmallScope"));
    

}


SmallScopeAudioProcessor::~SmallScopeAudioProcessor()
{
    
}

//==============================================================================
const String SmallScopeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SmallScopeAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SmallScopeAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double SmallScopeAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SmallScopeAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SmallScopeAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SmallScopeAudioProcessor::setCurrentProgram (int index)
{
}

const String SmallScopeAudioProcessor::getProgramName (int index)
{
    return String();
}

void SmallScopeAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SmallScopeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //RESET PARAMETERS
    oscilloscopeWaveform.setTriggerMode (*parameters.getRawParameterValue ("triggerMode"));
    oscilloscopeWaveform.setVerticalZoomFactor (*parameters.getRawParameterValue ("verticalZoom"));
    oscilloscopeWaveform.setNumSamplesPerPixel (*parameters.getRawParameterValue ("samplesPerPixel"));
    //oscilloscopeWaveform.setPhosphorDecayFactor (parameters.getRawParameterValue ("phosphorDecay"));
    oscilloscopeWaveform.setWaveformFillFactor (*parameters.getRawParameterValue ("waveformFill"));
    oscilloscopeWaveform.setTraceHue (*parameters.getRawParameterValue ("traceHue"));


}

void SmallScopeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SmallScopeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void SmallScopeAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (int i = totalNumInputChannels; i < (totalNumOutputChannels-1); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    for (int channel = 0; channel < (totalNumInputChannels -1); ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);

        oscilloscopeWaveform.addSamples(channelData, buffer.getNumSamples());
        
    }
}

//==============================================================================
bool SmallScopeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SmallScopeAudioProcessor::createEditor()
{
    return new SmallScopeAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void SmallScopeAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ScopedPointer<XmlElement> xml (parameters.state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SmallScopeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr){
        if (xmlState->hasTagName (parameters.state.getType())){
            parameters.state = ValueTree::fromXml (*xmlState);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SmallScopeAudioProcessor();
}

