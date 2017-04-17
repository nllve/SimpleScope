//
//  PhosphorescentLayer.h
//  SmallScope
//
//  Created by Jonathan Crawford on 4/8/17.
//
//

#ifndef PhosphorescentLayer_h
#define PhosphorescentLayer_h

#include "../JuceLibraryCode/JuceHeader.h"

class PhosphorescentLayer : public ImageComponent,
                            public Timer
{
public:
    
    PhosphorescentLayer(int depth);
    //PhosphorescentLayer(PhosphorescentLayer* layerPointer);
    ~PhosphorescentLayer();
    
    void updateLayer(Image& newImage);
    
    void updateNextLayer(Image& newImage);
    
    /** @internal */
    void resized() override;
    
    /** @internal */
    void paint (Graphics& g) override;
    
    /** @internal */
    void timerCallback() override;
    
    Image phosphorImage;
    
private:
    //==============================================================================
    int layerIndex;
    ScopedPointer<PhosphorescentLayer> nextLayer;
    bool lastLayer;
    
    bool readyForNextImage;
    CriticalSection phosphorLock;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhosphorescentLayer);
};

#endif /* PhosphorescentScreen_h */
