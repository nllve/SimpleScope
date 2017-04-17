//
//  PhosphorescentScreen.h
//  SmallScope
//
//  Created by Jonathan Crawford on 4/9/17.
//
//

#ifndef PhosphorescentScreen_h
#define PhosphorescentScreen_h

#include "../JuceLibraryCode/JuceHeader.h"
#include "PhosphorescentLayer.h"

class PhosphorescentScreen :    public Component, public Timer
{
public:
    
    PhosphorescentScreen(Image& inputReference);
    ~PhosphorescentScreen();
    
    void updateScreen();
    
    PhosphorescentScreen* getPointer();
    
    void resized() override;
    
    void paint (Graphics& g) override;
    
    void timerCallback() override;
    
    void setPhosphorDecayFactor (float newPhosphorDecayFactor);
    
    Image backgroundImage;
    Image& inputImage;
private:
    int numberOfLayers = 10;
    PhosphorescentLayer baseLayer;
    bool needToRepaint;
    
};

#endif /* PhosphorescentScreen_h */
