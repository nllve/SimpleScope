#ifndef __DROWAUDIO_WAVE_SCOPE_H__
#define __DROWAUDIO_WAVE_SCOPE_H__

#include "../JuceLibraryCode/JuceHeader.h"
#include "FifoBuffer.h"
#include "PhosphorescentScreen.h"

class WaveScope :   public Component,
                    public Timer,
                    public TimeSliceClient
{
public:
    //==============================================================================
    /** The enum to use when setting the trace trigger mode.
     */
    enum TriggerMode
    {
        Off,/**<< The trace will just refresh at a constant rate. */
        Up,     /**<< The start of the trace will be a rising edge. */
        Down    /**<< The start of the trace will be a falling edge. */
    };
    
    //==============================================================================
    /** Creates a Triggered scope.
        You should really specify a backGroundThread to use and make sure you start
        it before creating the scope. If you pass a nullptr in here it will create
        its own thread and manage its lifetime internally.
     */
    WaveScope (TimeSliceThread* backgroundAudioThreadToUse = nullptr);
    
    /** Destructor. */
    ~WaveScope();

    //==============================================================================
    void setNumSamplesPerPixel (int newNumSamplesPerPixel);
    
    void setVerticalZoomFactor (float newVerticalZoomFactor);
    
    void setTraceHue (float newTraceHue);
    
    void setWaveformFillFactor (float newWaveformFillFactor);
    
    void setTriggerMode (float newTriggerMode);
    
    WaveScope* getPointer();
    
    /** Adds a block of samples to the scope.
        
        Simply call this from your audio callback or similar to render the scope.
        This is a very quick method as it only takes a copy of the samples. All the
        processing and image rendering is performed on a background thread.
     */
    
    void addSamples (const float* samples, int numSamples);

    //==============================================================================
    /** @internal */
    void resized() override;
    
    /** @internal */
    void paint (Graphics& g) override;

    /** @internal */
    void timerCallback() override;
    
    /** @internal */
    int useTimeSlice() override;
    
    //PHOSPHORESCENT SCREEN
    ScopedPointer<PhosphorescentScreen> monitor;
    
private:
    //==============================================================================
    //PROCESSING THREAD
    OptionalScopedPointer<TimeSliceThread> backgroundAudioThreadToUse;

    //VISUAL PARAMETERS
    TriggerMode triggerMode;
    float verticalZoomFactor;
    int numSamplesPerPixel;
    float waveformFillFactor;
    Colour traceColour;
    
    //AUDIO PROCESSING MEMBERS
    int numLeftToAverage;
    int bufferSize, bufferWritePos;
    FifoBuffer<float> samplesToProcess;
    HeapBlock<float> tempProcessingBlock;
    float currentMin, currentMax;
    HeapBlock<float> minBuffer, maxBuffer;
    
    //IMAGE LOCK
    Image outputImage;
    bool needToUpdate;
    bool needToRepaint;
    CriticalSection imageLock;
    


    //==============================================================================
    void processPendingSamples();
    void renderImage();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveScope);
};

#endif  // __DROWAUDIO_WAVE_SCOPE_H__
