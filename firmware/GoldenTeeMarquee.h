#ifndef _GOLDENTEEMARQUEE_H
#define _GOLDENTEEMARQUEE_H

#include "application.h"
#include "GoldenTeeFrameBuffer.h"
// This #include statement was automatically added by the Particle IDE.
#include <SparkIntervalTimer.h>

enum LED_STATE 
{ 
  LED_STATE_INIT, 
  LED_STATE_SEND_ROW_DATA, 
  //LED_STATE_LOAD_BYTE_DATA, 
  //LED_STATE_PREPARE_BIT, 
  //LED_STATE_LATCH_BIT, 
  //LED_STATE_END_ROW,
  //LED_STATE_POWER_OFF_DELAY,
  LED_STATE_NEXT_ROW, 
}; 



class GoldenTeeMarquee
{
    public:
	    GoldenTeeMarquee();
	    ~GoldenTeeMarquee();
	    
	    static GoldenTeeMarquee* Create();
	    static void DoTick();
	    static void Pause();
	    static void Resume();
	    
	    void Init();
	    void Tick();
	    void SubmitFrameBuffer(GoldenTeeFrameBuffer *frameBuffer);
        GoldenTeeFrameBuffer* GetFrameBuffer();
        
        int GetFrameBufferHeight();
        int GetFrameBufferWidth();
        
        byte GetBitOffIndex();
        void SetBitOffIndex(byte index);
        void SetRefreshPerFrame(byte count);
    private:


    
    	//byte frameBuffer2[LED_ROW_ENABLE_COUNT*2 * LED_ROW_BYTE_COUNT];
        //GoldenTeeFrameBuffer *frameBuffer[2] = {NULL, NULL};
        GoldenTeeFrameBuffer *writeBuffer, *readBuffer;
    

    
        //byte **frontBuffer = &frameBuffer[0];
        //byte backBuffer** = frameBuffer2;
    
        volatile LED_STATE currentState; 
        volatile byte stateCounter, currentRow, bitIndex, bitPowerOff, refreshCount, lastRow, refreshPerFrame; 

        volatile bool frameBufferSwitching;
};

#endif