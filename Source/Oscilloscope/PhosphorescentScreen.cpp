//
//  PhosphorescentScreen.cpp
//  SmallScope
//
//  Created by Jonathan Crawford on 4/9/17.
//
//

#include "PhosphorescentScreen.h"

//==============================================================================
PhosphorescentScreen::PhosphorescentScreen(Image& inputReference)
:    inputImage (inputReference), baseLayer(10)
{
    setSize (512, 512);
    backgroundImage = Image (Image::ARGB, 512, 512, false);
    inputImage = Image (Image::ARGB, 512, 512, false);
    
    addAndMakeVisible(baseLayer);
    
    needToRepaint = false;
    startTimerHz (30);
}


//==============================================================================
PhosphorescentScreen::~PhosphorescentScreen()
{
    stopTimer();
}


//==============================================================================
PhosphorescentScreen* PhosphorescentScreen::getPointer()
{
    return this;
}


//==============================================================================
void PhosphorescentScreen::resized()
{
    setBounds (0, 0, 512, 512);
    backgroundImage = Image (Image::ARGB, 512, 512, false);
    inputImage = Image (Image::ARGB, 512, 512, false);
}


//==============================================================================
void PhosphorescentScreen::paint(Graphics& g)
{
    g.drawImageAt (backgroundImage, 0, 0);
}


//==============================================================================
void PhosphorescentScreen::timerCallback()
{
    if (needToRepaint)
    {
        repaint();
        needToRepaint = false;
    }
}


//==============================================================================
void PhosphorescentScreen::setPhosphorDecayFactor (float newPhosphorDecayFactor)
{
}


//==============================================================================
void PhosphorescentScreen::updateScreen()
{
    baseLayer.updateLayer(inputImage);
    needToRepaint = true;
}
