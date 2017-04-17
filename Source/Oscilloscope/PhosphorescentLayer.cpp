//
//  PhosphorescentLayer.cpp
//  SmallScope
//
//  Created by Jonathan Crawford on 4/8/17.
//
//

#include "PhosphorescentLayer.h"

PhosphorescentLayer::PhosphorescentLayer(int index)
: phosphorImage (Image (Image::ARGB, 512, 512, false))
{
    layerIndex = index-1;
    setSize (512, 512);
    if (layerIndex > 1)
    {
        nextLayer = new PhosphorescentLayer(layerIndex);
        addAndMakeVisible (nextLayer);
        
    }
    startTimerHz (30);
}
/**
PhosphorescentLayer::PhosphorescentLayer(PhosphorescentLayer* layerPointer)
: ImageComponent(), phosphorImage (Image (Image::ARGB, 512, 512, false)), nextLayer (layerPointer)
{
    setSize (512, 512);
    
    setImage (phosphorImage);
    
    startTimerHz (30);
}
 */

PhosphorescentLayer::~PhosphorescentLayer()
{
}

void PhosphorescentLayer::resized()
{
    setBounds (0, 0, 512, 512);
}

//==============================================================================
void PhosphorescentLayer::paint (Graphics& g)
{
    const ScopedLock sl (phosphorLock);
    g.drawImageAt(phosphorImage, 0, 0);
    
}

void PhosphorescentLayer::timerCallback()
{
    if (!readyForNextImage)
    {
        repaint();
        readyForNextImage = true;
    }
}

void PhosphorescentLayer::updateLayer(Image& newImage)
{
    const ScopedLock sl (phosphorLock);
    if (readyForNextImage)
    {
        if(!lastLayer)
        {
            updateNextLayer(phosphorImage);
            phosphorImage = newImage;
        }
        readyForNextImage = false;
    }

}

void PhosphorescentLayer::updateNextLayer(Image& newImage)
{
    nextLayer->updateLayer(newImage);
}


