/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"


//==============================================================================
SmallScopeAudioProcessorEditor::SmallScopeAudioProcessorEditor (SmallScopeAudioProcessor& p, AudioProcessorValueTreeState& vts)
:   AudioProcessorEditor (&p), processor (p), valueTreeState (vts),
    oscilloscopeWaveform (processor.oscilloscopeWaveform.getPointer()), triggerModeButtonLogic (vts)
{
    addAndMakeVisible (oscilloscopeWaveform);
    

    //BUTTONS
    //----TRIGGER OFFF
    triggerOffButton = new ImageButton ("TriggerOffButton");
    triggerOffButton->setSize (64, 64);
    triggerOffButton->setImages (true, false, true, exampleImage1, 1.0f,/* opacity when normal*/
                                 /*normal color overlay*/   Colour (0.0f,0.0f,0.0f,0.0f), exampleImage2, 0.75f,
                                 /*over color overlay*/     Colour (0.0f,0.0f,0.0f,0.0f), exampleImage3, 1.0f,
                                 /*down color overlay*/     Colour (0.0f,0.0f,0.0f,0.0f));
    addAndMakeVisible (triggerOffButton);
    //----TRIGGER UP
    triggerUpButton = new ImageButton ("TriggerUpButton");
    triggerUpButton->setSize (64, 64);
    triggerUpButton->setImages (true, false, true, exampleImage1, 1.0f,/* opacity when normal*/
                                /*normal color overlay*/   Colour (0.0f,0.0f,0.0f,0.0f), exampleImage2, 0.75f,
                                /*over color overlay*/     Colour (0.0f,0.0f,0.0f,0.0f), exampleImage3, 1.0f,
                                /*down color overlay*/     Colour (0.0f,0.0f,0.0f,0.0f));
    addAndMakeVisible (triggerUpButton);
    //----TRIGGER DOWN
    triggerDownButton = new ImageButton ("triggerDownButton");
    triggerDownButton->setSize (64, 64);
    triggerDownButton->setImages (true, false, true, exampleImage1, 1.0f,/* opacity when normal*/
                                  /*normal color overlay*/   Colour (0.0f,0.0f,0.0f,0.0f), exampleImage2, 0.75f,
                                  /*over color overlay*/     Colour (0.0f,0.0f,0.0f,0.0f), exampleImage3, 1.0f,
                                  /*down color overlay*/     Colour (0.0f,0.0f,0.0f,0.0f));
    addAndMakeVisible (triggerDownButton);
    
    triggerModeButtonLogicSlider = triggerModeButtonLogic.returnAttachedSlider (*(triggerOffButton), *(triggerUpButton), *(triggerDownButton));
    triggerModeButtonLogicSlider->addListener (this);
    
    
        //SLIDERS
    //----VERTICAL ZOOM
    addAndMakeVisible (verticalZoomSlider);
    verticalZoomSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    //verticalZoomSlider.setRange (1.0f, 2.0f);
    verticalZoomSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    verticalZoomSlider.addListener(this);
    //----SAMPLES PER PIXEL
    addAndMakeVisible (samplesPerPixelSlider);
    samplesPerPixelSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    //samplesPerPixelSlider.setRange(2.0f, 256.0f);
    samplesPerPixelSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    samplesPerPixelSlider.addListener(this);
    //----PHOSPHOR DECAY
    addAndMakeVisible (phosphorDecaySlider);
    phosphorDecaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    //phosphorDecaySlider.setRange(0.005f, 0.3f);
    phosphorDecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    phosphorDecaySlider.addListener(this);
    //----WAVEFORM FILL
    addAndMakeVisible (waveformFillSlider);
    waveformFillSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    //waveformFillSlider.setRange(0.0f, 0.028f);
    waveformFillSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    waveformFillSlider.addListener(this);
    //----TRACE HUE
    addAndMakeVisible (traceHueSlider);
    traceHueSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    //traceHueSlider.setRange(0.0f, 0.15f);
    traceHueSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    traceHueSlider.addListener(this);
    
    
    //SLIDER ATTACHMENTS
    //----VERTICAL ZOOM
    verticalZoomAttachment = new SliderAttachment (valueTreeState, "verticalZoom", verticalZoomSlider);
    //----SAMPLES PER PIXEL
    samplesPerPixelAttachment = new SliderAttachment (valueTreeState, "samplesPerPixel", samplesPerPixelSlider);
    //----PHOSPHOR DECAY
    phosphorDecayAttachment = new SliderAttachment (valueTreeState, "phosphorDecay", phosphorDecaySlider);
    //----WAVEFORM FILL
    waveformFillAttachment = new SliderAttachment (valueTreeState, "waveformFill", waveformFillSlider);
    //----TRACE HUE
    traceHueAttachment = new SliderAttachment (valueTreeState, "traceHue", traceHueSlider);
    
    
    setSize (512, 512); /*** must give editor size before returning it */
}

SmallScopeAudioProcessorEditor::~SmallScopeAudioProcessorEditor()
{
    
    
}

//==============================================================================
void SmallScopeAudioProcessorEditor::paint (Graphics& g)
{
}

//==============================================================================
void SmallScopeAudioProcessorEditor::resized()
{
    
    //OSCILLOSCOPE
    oscilloscopeWaveform->setBounds (0, 0, 512, 512);
    
    //BUTTONS
    triggerOffButton->setBounds (12, 436, 64, 64);
    triggerUpButton->setBounds (76, 436, 64, 64);
    triggerDownButton->setBounds (140, 436, 64, 64);
    
    
    //SLIDERS
    verticalZoomSlider.setBounds (100, 0, 50, 50);
    samplesPerPixelSlider.setBounds (150, 0, 50, 50);
    phosphorDecaySlider.setBounds (200, 0, 50, 50);
    waveformFillSlider.setBounds (250, 0, 50, 50);
    traceHueSlider.setBounds (50, 0, 50, 50);
    
}

//==============================================================================
void SmallScopeAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (slider == triggerModeButtonLogicSlider)
    {
        oscilloscopeWaveform->setTriggerMode (slider->getValue());
    }
    
    else if (slider == &verticalZoomSlider)
    {
        oscilloscopeWaveform->setVerticalZoomFactor (slider->getValue());
        verticalZoomSlider.setValue (verticalZoomSlider.getValue(), dontSendNotification);
    }
    
    else if (slider == &samplesPerPixelSlider)
    {
        oscilloscopeWaveform->setNumSamplesPerPixel (slider->getValue());
        samplesPerPixelSlider.setValue (samplesPerPixelSlider.getValue(), dontSendNotification);
    }
    
    else if (slider == &phosphorDecaySlider)
    {
        oscilloscopeWaveform->monitor->setPhosphorDecayFactor (slider->getValue());
        phosphorDecaySlider.setValue(phosphorDecaySlider.getValue(), dontSendNotification);
    }
    
    else if (slider == &waveformFillSlider)
    {
        oscilloscopeWaveform->setWaveformFillFactor (slider->getValue());
        waveformFillSlider.setValue (waveformFillSlider.getValue(), dontSendNotification);
    }
    
    else if (slider == &traceHueSlider)
    {
        oscilloscopeWaveform->setTraceHue (slider->getValue());
        traceHueSlider.setValue (traceHueSlider.getValue(), dontSendNotification);
    }
}







