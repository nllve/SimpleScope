/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "PhosphorescentScreen.h"
#include "RadioButtonLogic.h"



//==============================================================================
/**
*/
typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

class SmallScopeAudioProcessorEditor  : public AudioProcessorEditor,
                                        public Slider::Listener
{
public:
    SmallScopeAudioProcessorEditor (SmallScopeAudioProcessor&, AudioProcessorValueTreeState&);
    ~SmallScopeAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void sliderValueChanged (Slider* slider) override;
    
    
private:
    SmallScopeAudioProcessor& processor;                    //refer to the processor
    AudioProcessorValueTreeState& valueTreeState;           //refer to the parameters
    
    
    //DISPLAY
    WaveScope* oscilloscopeWaveform;
    
    //BUTTON IMAGES
    Image exampleImage1 = ImageCache::getFromMemory (BinaryData::example_1_png, BinaryData::example_1_pngSize);
    Image exampleImage2 = ImageCache::getFromMemory (BinaryData::example_2_png, BinaryData::example_2_pngSize);
    Image exampleImage3 = ImageCache::getFromMemory (BinaryData::example_3_png, BinaryData::example_3_pngSize);
    
    
    //BUTTONS
    ScopedPointer<ImageButton> triggerOffButton;
    ScopedPointer<ImageButton> triggerUpButton;
    ScopedPointer<ImageButton> triggerDownButton;
    
    
    //BUTTON GROUP
    Slider* triggerModeButtonLogicSlider;
    RadioButtonLogic triggerModeButtonLogic;
    
    //SLIDERS
    Slider verticalZoomSlider;
    Slider samplesPerPixelSlider;
    Slider phosphorDecaySlider;
    Slider waveformFillSlider;
    Slider traceHueSlider;
    
    //SLIDER ATTACHMENTS
    ScopedPointer<SliderAttachment> verticalZoomAttachment;
    ScopedPointer<SliderAttachment> samplesPerPixelAttachment;
    ScopedPointer<SliderAttachment> phosphorDecayAttachment;
    ScopedPointer<SliderAttachment> waveformFillAttachment;
    ScopedPointer<SliderAttachment> traceHueAttachment;

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SmallScopeAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
