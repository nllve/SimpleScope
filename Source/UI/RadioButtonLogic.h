/*
  ==============================================================================

    RadioButtonGroup.h
    Created: 14 Apr 2017 7:54:46am
    Author:  Jonathan Crawford

  ==============================================================================
*/

#ifndef RadioButtonLogic_h
#define RadioButtonLogic_h

#include "../JuceLibraryCode/JuceHeader.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
class RadioButtonLogic :    public Slider::Listener,
                            public Button::Listener
{
public:
    RadioButtonLogic(AudioProcessorValueTreeState& vts)
    :   valueTreeState (vts)
    {
        parameter = valueTreeState.getParameter("triggerMode");
        
        radioButtonGroupSlider = new Slider();

    };
    
    ~RadioButtonLogic(){
        example_1->removeListener(this);
        example_2->removeListener(this);
        example_3->removeListener(this);
    };


/** monitors plugin parameter state*/
void sliderValueChanged(Slider* slider) override
{
    const ScopedLock sl (radioButtonLock);
    
    if (slider->getValue() >= 0.1 && slider->getValue() < 0.333f)
    {
        example_1->setToggleState(true, dontSendNotification);
        
        example_1->setClickingTogglesState(false);
        example_2->setClickingTogglesState(true);
        example_3->setClickingTogglesState(true);
        
        if (example_2->isMouseOver())
            example_2->setState(Button::ButtonState::buttonOver);
        else
            example_2->setState(Button::ButtonState::buttonNormal);
        
        if (example_3->isMouseOver())
            example_3->setState(Button::ButtonState::buttonOver);
        
        else
            example_3->setState(Button::ButtonState::buttonNormal);
        
    }
    else if (slider->getValue() >= 0.333f && slider->getValue() < 0.666f)
    {
        example_2->setToggleState(true, dontSendNotification);
        
        example_1->setClickingTogglesState(true);
        example_2->setClickingTogglesState(false);
        example_3->setClickingTogglesState(true);
        
        if (example_1->isMouseOver())
            example_1->setState(Button::ButtonState::buttonOver);
        else
            example_1->setState(Button::ButtonState::buttonNormal);
        
        if (example_3->isMouseOver())
            example_3->setState(Button::ButtonState::buttonOver);
        
        else
            example_3->setState(Button::ButtonState::buttonNormal);
    }
    else if (slider->getValue() >= 0.666f)
    {
        example_3->setToggleState(true, dontSendNotification);
        
        example_1->setClickingTogglesState(true);
        example_2->setClickingTogglesState(true);
        example_3->setClickingTogglesState(false);
        
        if (example_1->isMouseOver())
            example_1->setState(Button::ButtonState::buttonOver);
        else
            example_1->setState(Button::ButtonState::buttonNormal);
        
        if (example_2->isMouseOver())
            example_2->setState(Button::ButtonState::buttonOver);
        
        else
            example_2->setState(Button::ButtonState::buttonNormal);
    }
};
    
/** watches for user button clicks*/
void buttonClicked (Button* button) override
{
    const ScopedLock sl (radioButtonLock);
    if (button == example_1)
    {
        if (example_1->getState() == Button::ButtonState::buttonOver)
        {
            example_1->setToggleState(true, dontSendNotification);
            
            example_1->setClickingTogglesState(false);
            example_2->setClickingTogglesState(true);
            example_3->setClickingTogglesState(true);
            parameter->setValueNotifyingHost (0.15f);
        }
        else if (example_1->getState() == Button::ButtonState::buttonDown)
        {
            parameter->setValueNotifyingHost (0.15f);
        }
    }
    else if (button == example_2)
    {
        if (example_2->getState() == Button::ButtonState::buttonOver)
        {
            example_2->setToggleState(true, dontSendNotification);
            
            example_1->setClickingTogglesState(true);
            example_2->setClickingTogglesState(false);
            example_3->setClickingTogglesState(true);
            parameter->setValueNotifyingHost (0.45f);
        }
        else if (example_1->getState() == Button::ButtonState::buttonDown)
        {
            parameter->setValueNotifyingHost (0.45f);
        }
    }
    else if (button == example_3)
    {
        if (example_3->getState() == Button::ButtonState::buttonOver)
        {
            example_3->setToggleState(true, dontSendNotification);
            
            example_1->setClickingTogglesState(true);
            example_2->setClickingTogglesState(true);
            example_3->setClickingTogglesState(false);
            parameter->setValueNotifyingHost (0.75f);
        }
        else if (example_1->getState() == Button::ButtonState::buttonDown)
        {
            parameter->setValueNotifyingHost (0.75f);
        }
    }
};
    
Slider* returnAttachedSlider (Button& btn1, Button& btn2, Button& btn3)
{
    example_1 = &btn1;
    example_2 = &btn2;
    example_3 = &btn3;
    
    example_1->addListener(this);
    example_2->addListener(this);
    example_3->addListener(this);
    example_1->setRadioGroupId(1, sendNotification);
    example_2->setRadioGroupId(1, sendNotification);
    example_3->setRadioGroupId(1, sendNotification);
    example_1->setClickingTogglesState(true);
    example_2->setClickingTogglesState(true);
    example_3->setClickingTogglesState(true);
    
    radioButtonGroupSlider->setRange(0.1f, 1.0f);
    radioButtonGroupSlider->addListener(this);

    radioButtonGroupSliderAttachment = new SliderAttachment (valueTreeState, "triggerMode", *radioButtonGroupSlider);
    
    
    
    parameter->setValueNotifyingHost (parameter->getValue());
    
    return radioButtonGroupSlider.get();
};

private:
    AudioProcessorValueTreeState& valueTreeState;
    AudioProcessorParameter* parameter;
    float currentValue;
    Button* example_1;
    Button* example_2;
    Button* example_3;
    
    ScopedPointer<Slider> radioButtonGroupSlider;
    ScopedPointer<SliderAttachment> radioButtonGroupSliderAttachment;
    
    CriticalSection radioButtonLock;

//==============================================================================
JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RadioButtonLogic);
};

#endif /* RadioButtonLogic_h */
