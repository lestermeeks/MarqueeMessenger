#include "GoldenTeeMarquee.h"




#define SR_ROW_1_ENABLE A1  
#define SR_ROW_2_ENABLE A0 
#define SR_ROW_3_ENABLE A2 
#define SR_ROW_4_ENABLE A4 
#define SR_ROW_5_ENABLE DAC 
#define SR_ROW_6_ENABLE A3
#define SR_ROW_7_ENABLE A5 

#define SR_LATCH_PIN D5 
#define SR_OUTPUT_PIN D6 
#define SR_TOP_CLK_PIN D1 
#define SR_TOP_DAT_PIN D3 
#define SR_BTM_CLK_PIN D2 
#define SR_BTM_DAT_PIN D4 

#define SR_LATCH_RESET pinResetFast(SR_LATCH_PIN) 
#define SR_LATCH_ENABLE pinSetFast(SR_LATCH_PIN) 

#define SR_OUTPUT_ENABLE pinResetFast(SR_OUTPUT_PIN) 
#define SR_OUTPUT_DISABLE pinSetFast(SR_OUTPUT_PIN) 

#define SR_TOP_CLK_RESET pinResetFast(SR_TOP_CLK_PIN) 
#define SR_TOP_CLK_LATCH pinSetFast(SR_TOP_CLK_PIN) 

#define SR_TOP_DAT_HIGH pinSetFast(SR_TOP_DAT_PIN) 
#define SR_TOP_DAT_LOW pinResetFast(SR_TOP_DAT_PIN) 

#define SR_BTM_CLK_RESET pinResetFast(SR_BTM_CLK_PIN) 
#define SR_BTM_CLK_LATCH pinSetFast(SR_BTM_CLK_PIN) 

#define SR_BTM_DAT_HIGH pinSetFast(SR_BTM_DAT_PIN) 
#define SR_BTM_DAT_LOW pinResetFast(SR_BTM_DAT_PIN)

// timer duraction in uS
#define TIMER_DURATION 1000
//#define PAUSE_DURATION 500
#define BIT_DURATION 2

#define ROW_INTERLACE_STEP 1

const int LED_ROW_ENABLE_COUNT = 7; 
const int LED_ROW_BIT_COUNT = 70; 

const byte MIN_OFF_INDEX = 1;
const byte MAX_OFF_INDEX = 60;


//const int LED_ROW_BYTE_COUNT = 9; //9x8 
const int LED_ROW_ENABLE[LED_ROW_ENABLE_COUNT] = { 
  SR_ROW_1_ENABLE, 
  SR_ROW_2_ENABLE, 
  SR_ROW_3_ENABLE, 
  SR_ROW_4_ENABLE, 
  SR_ROW_5_ENABLE, 
  SR_ROW_6_ENABLE,
  SR_ROW_7_ENABLE
  }; 

GoldenTeeMarquee *singleton = NULL;
IntervalTimer *timer = NULL;		// 3 for the Core


GoldenTeeMarquee* GoldenTeeMarquee::Create()
{
    if(singleton == NULL)
    
    {
        singleton = new GoldenTeeMarquee();
        singleton->Init();
        
        timer = new IntervalTimer();
        timer->begin(GoldenTeeMarquee::DoTick, TIMER_DURATION, uSec);
    }
    
    return singleton;
}

void GoldenTeeMarquee::Pause()
{
    //timer->end();
    //timer->begin(GoldenTeeMarquee::DoTick, PAUSE_DURATION, uSec);
}

void GoldenTeeMarquee::Resume()
{
    //timer->end();
    //timer->begin(GoldenTeeMarquee::DoTick, TIMER_DURATION, uSec);
}

void GoldenTeeMarquee::DoTick()
{
    singleton->Tick();
}


GoldenTeeMarquee::GoldenTeeMarquee()
{
    
}

GoldenTeeMarquee::~GoldenTeeMarquee()
{
}


void GoldenTeeMarquee::Init()
{
    
    
    writeBuffer = new GoldenTeeFrameBuffer();//LED_ROW_BIT_COUNT, LED_ROW_ENABLE_COUNT*2);
    readBuffer = new GoldenTeeFrameBuffer();//LED_ROW_BIT_COUNT, LED_ROW_ENABLE_COUNT*2);
    frameBufferSwitching = false;
    
    currentRow = 0;
    lastRow = 0;
    currentState = LED_STATE_INIT; 
    
    // declare pin 9 to be an output: 
    pinMode(SR_LATCH_PIN, OUTPUT); 
    SR_LATCH_RESET; 
    
    pinMode(SR_OUTPUT_PIN, OUTPUT); 
    SR_OUTPUT_DISABLE; 
    
    pinMode(SR_TOP_CLK_PIN, OUTPUT); 
    pinMode(SR_TOP_DAT_PIN, OUTPUT); 
    pinMode(SR_BTM_CLK_PIN, OUTPUT); 
    pinMode(SR_BTM_DAT_PIN, OUTPUT); 

    //all power pins out and off 
    for(int rowIdx = 0; rowIdx <LED_ROW_ENABLE_COUNT; rowIdx++) 
    { 
        pinMode( LED_ROW_ENABLE[rowIdx], OUTPUT); 
        digitalWrite (LED_ROW_ENABLE[rowIdx], LOW); 
    }
    
    refreshPerFrame = 1;
    bitIndex = 0;
    bitPowerOff = MAX_OFF_INDEX;
    refreshCount = 0;

}

void GoldenTeeMarquee::Tick()
{
        
    switch(currentState) 
    { 
        default: 
        case LED_STATE_INIT: 
            SR_OUTPUT_ENABLE; 
            currentState = LED_STATE_SEND_ROW_DATA;
            break; 
 
 
        case LED_STATE_SEND_ROW_DATA: 
            bitIndex = 0; 
        //case LED_STATE_PREPARE_BIT: 
            //pinResetFast(LED_ROW_ENABLE[lastRow]);
            pinResetFast(LED_ROW_ENABLE[lastRow]);
            SR_LATCH_RESET;
            while(bitIndex < LED_ROW_BIT_COUNT)
            {
                SR_TOP_CLK_RESET;
                SR_BTM_CLK_RESET; 
                
                if(readBuffer->GetPixelFast(bitIndex, currentRow)) 
                    SR_TOP_DAT_HIGH; 
                else 
                    SR_TOP_DAT_LOW; 
                
    
                if( readBuffer->GetPixelFast(bitIndex, currentRow+LED_ROW_ENABLE_COUNT)) 
                    SR_BTM_DAT_HIGH; 
                else 
                    SR_BTM_DAT_LOW; 
             
                delayMicroseconds(BIT_DURATION);
    
                SR_TOP_CLK_LATCH; 
                SR_BTM_CLK_LATCH; 
    
                delayMicroseconds(BIT_DURATION);
                
                bitIndex++; 

            }
            
            SR_LATCH_ENABLE; 
            
            pinSetFast(LED_ROW_ENABLE[currentRow]);
            lastRow = currentRow;
            
            //currentState = LED_STATE_NEXT_ROW;
            //break; 

        //case LED_STATE_NEXT_ROW: 


             
            //setup our vars to shift in the next row 
            currentRow += ROW_INTERLACE_STEP; 
            if( currentRow >= LED_ROW_ENABLE_COUNT)
            {
                currentRow -= LED_ROW_ENABLE_COUNT; 
            }
            
            if(currentRow == 0){
                if(++refreshCount >= refreshPerFrame && frameBufferSwitching)
                {
                    
                    refreshCount =0;
                    //we at row zero?  and have a new writebuffer to use? 
                    //if(currentRow == 0 && frameBufferSwitching)
                    {
                        GoldenTeeFrameBuffer *temp = writeBuffer;
                        writeBuffer = readBuffer;
                        readBuffer = temp;
                        frameBufferSwitching = false;
                    }
                }
            }
            
            currentState = LED_STATE_SEND_ROW_DATA; 
            
            break; 
  } 
}

void GoldenTeeMarquee::SubmitFrameBuffer(GoldenTeeFrameBuffer *frameBuffer)
{
    if(!frameBufferSwitching && frameBuffer == writeBuffer)
    {
        frameBufferSwitching = true;
    }
}

GoldenTeeFrameBuffer* GoldenTeeMarquee::GetFrameBuffer()
{
    if(!frameBufferSwitching)
    {
        return writeBuffer;
    }
    else
    {
        return NULL;
    }
}

int GoldenTeeMarquee::GetFrameBufferHeight()
{
    return LED_ROW_ENABLE_COUNT*2;
}

int GoldenTeeMarquee::GetFrameBufferWidth()
{
    return LED_ROW_BIT_COUNT;
}

void GoldenTeeMarquee::SetBitOffIndex(byte index)
{
    bitPowerOff = index;
    
    if(bitPowerOff < MIN_OFF_INDEX)
    bitPowerOff = MIN_OFF_INDEX;
    if(bitPowerOff>MAX_OFF_INDEX)
    bitPowerOff = MAX_OFF_INDEX;
}

void GoldenTeeMarquee::SetRefreshPerFrame(byte count)
{
    if(count == 0)
        refreshPerFrame = 1;
    else
        refreshPerFrame = count;
}

byte GoldenTeeMarquee::GetBitOffIndex()
{
    return bitPowerOff;
}
