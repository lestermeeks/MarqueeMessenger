// This #include statement was automatically added by the Particle IDE.
#include "TinyGPS/TinyGPS.h"
#include "SparkIntervalTimer/SparkIntervalTimer.h"


#define TIMER_DURATION 25
#define DEBUG_LED D7

enum LED_STATE
{
  LED_STATE_INIT,
  LED_STATE_RESET_ROW_DATA,
  LED_STATE_LOAD_BYTE_DATA,
  LED_STATE_PREPARE_BIT,
  LED_STATE_LATCH_BIT,
  LED_STATE_END_ROW,
  LED_STATE_NEXT_ROW,
};

#define SR_ROW_0_ENABLE C0
#define SR_ROW_1_ENABLE C1
#define SR_ROW_2_ENABLE C2
#define SR_ROW_3_ENABLE C3
#define SR_ROW_4_ENABLE C4
#define SR_ROW_5_ENABLE C5
#define SR_ROW_6_ENABLE D0

#define SR_LATCH_PIN B0
#define SR_OUTPUT_PIN B1
#define SR_TOP_CLK_PIN B2
#define SR_TOP_DAT_PIN B3
#define SR_BTM_CLK_PIN B4
#define SR_BTM_DAT_PIN B5

#define SR_LATCH_RESET digitalWrite(SR_LATCH_PIN, LOW)
#define SR_LATCH_ENABLE digitalWrite(SR_LATCH_PIN, HIGH)

#define SR_OUTPUT_ENABLE digitalWrite(SR_OUTPUT_PIN, LOW)
#define SR_OUTPUT_DISABLE digitalWrite(SR_OUTPUT_PIN, HIGH)

#define SR_TOP_CLK_RESET digitalWrite(SR_TOP_CLK_PIN, LOW)
#define SR_TOP_CLK_LATCH digitalWrite(SR_TOP_CLK_PIN, HIGH)

#define SR_TOP_DAT_HIGH digitalWrite(SR_TOP_DAT_PIN, HIGH)
#define SR_TOP_DAT_LOW digitalWrite(SR_TOP_DAT_PIN, LOW)

#define SR_BTM_CLK_RESET digitalWrite(SR_BTM_CLK_PIN, LOW)
#define SR_BTM_CLK_LATCH digitalWrite(SR_BTM_CLK_PIN, HIGH)

#define SR_BTM_DAT_HIGH digitalWrite(SR_BTM_DAT_PIN, HIGH)
#define SR_BTM_DAT_LOW digitalWrite(SR_BTM_DAT_PIN, LOW)

const int LED_ROW_ENABLE_COUNT = 7;
const int LED_ROW_BIT_COUNT = 70;
const int LED_ROW_BYTE_COUNT = 9; //9x8
const int LED_ROW_ENABLE[LED_ROW_ENABLE_COUNT] = {
  SR_ROW_0_ENABLE,
  SR_ROW_1_ENABLE,
  SR_ROW_2_ENABLE,
  SR_ROW_3_ENABLE,
  SR_ROW_4_ENABLE,
  SR_ROW_5_ENABLE,
  SR_ROW_6_ENABLE
  };

byte frameBuffer[LED_ROW_ENABLE_COUNT*2][ LED_ROW_BYTE_COUNT ];
volatile LED_STATE currentState;
volatile byte stateCounter, currentRow;
volatile byte currentUpperData, currentLowerData, bitIndex, byteIndex, bitMask;
byte test;
bool fDebugState = false;

unsigned long prevTicks = 0;

#define CHAR_WIDTH 5
#define CHAR_HEIGHT 7

#define CHAR_OFFSET 0x20
#define CHAR_COUNT 96

#define CHAR_DATA(a) (byte*)( FontData[(byte)a-CHAR_OFFSET] )
// #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_byte_near(addr) (*(const unsigned char *)(addr))

#define TOP_MESSAGE_SIZE 128
char TopBuffer[TOP_MESSAGE_SIZE+1] = "TOP";
#define BTM_MESSAGE_SIZE 128
char BtmBuffer[BTM_MESSAGE_SIZE+1] = "BTM";

byte FontBuffer[CHAR_HEIGHT];

const PROGMEM byte FontData[CHAR_COUNT][CHAR_HEIGHT] = {

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // 0x20, Space
    {0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04},   // 0x21, !
    {0x09, 0x09, 0x12, 0x00, 0x00, 0x00, 0x00},   // 0x22, "
    {0x0a, 0x0a, 0x1f, 0x0a, 0x1f, 0x0a, 0x0a},   // 0x23, #
    {0x04, 0x0f, 0x14, 0x0e, 0x05, 0x1e, 0x04},   // 0x24, $
    {0x19, 0x19, 0x02, 0x04, 0x08, 0x13, 0x13},   // 0x25, %
    {0x04, 0x0a, 0x0a, 0x0a, 0x15, 0x12, 0x0d},   // 0x26, &
    {0x04, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00},   // 0x27, '
    {0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02},   // 0x28, (
    {0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08},   // 0x29, )
    {0x04, 0x15, 0x0e, 0x1f, 0x0e, 0x15, 0x04},   // 0x2a, *
    {0x00, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x00},   // 0x2b, +
    {0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x08},   // 0x2c, ,
    {0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00},   // 0x2d, -
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c},   // 0x2e, .
    {0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10},   // 0x2f, /
    {0x0e, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0e},   // 0x30, 0
    {0x04, 0x0c, 0x04, 0x04, 0x04, 0x04, 0x0e},   // 0x31, 1
    {0x0e, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1f},   // 0x32, 2
    {0x0e, 0x11, 0x01, 0x06, 0x01, 0x11, 0x0e},   // 0x33, 3
    {0x02, 0x06, 0x0a, 0x12, 0x1f, 0x02, 0x02},   // 0x34, 4
    {0x1f, 0x10, 0x1e, 0x01, 0x01, 0x11, 0x0e},   // 0x35, 5
    {0x06, 0x08, 0x10, 0x1e, 0x11, 0x11, 0x0e},   // 0x36, 6
    {0x1f, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},   // 0x37, 7
    {0x0e, 0x11, 0x11, 0x0e, 0x11, 0x11, 0x0e},   // 0x38, 8
    {0x0e, 0x11, 0x11, 0x0f, 0x01, 0x02, 0x0c},   // 0x39, 9
    {0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x0c, 0x00},   // 0x3a, :
    {0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x04, 0x08},   // 0x3b, ;
    {0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02},   // 0x3c, <
    {0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00},   // 0x3d, =
    {0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08},   // 0x3e, >
    {0x0e, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04},   // 0x3f, ?

    {0x0e, 0x11, 0x17, 0x15, 0x17, 0x10, 0x0f},   // 0x40, @
    {0x04, 0x0a, 0x11, 0x11, 0x1f, 0x11, 0x11},   // 0x41, A
    {0x1e, 0x11, 0x11, 0x1e, 0x11, 0x11, 0x1e},   // 0x42, B
    {0x0e, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0e},   // 0x43, C
    {0x1e, 0x09, 0x09, 0x09, 0x09, 0x09, 0x1e},   // 0x44, D
    {0x1f, 0x10, 0x10, 0x1c, 0x10, 0x10, 0x1f},   // 0x45, E
    {0x1f, 0x10, 0x10, 0x1f, 0x10, 0x10, 0x10},   // 0x46, F
    {0x0e, 0x11, 0x10, 0x10, 0x13, 0x11, 0x0f},   // 0x37, G
    {0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11},   // 0x48, H
    {0x0e, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0e},   // 0x49, I
    {0x1f, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0c},   // 0x4a, J
    {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11},   // 0x4b, K
    {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f},   // 0x4c, L
    {0x11, 0x1b, 0x15, 0x11, 0x11, 0x11, 0x11},   // 0x4d, M
    {0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11},   // 0x4e, N
    {0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e},   // 0x4f, O
    {0x1e, 0x11, 0x11, 0x1e, 0x10, 0x10, 0x10},   // 0x50, P
    {0x0e, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0d},   // 0x51, Q
    {0x1e, 0x11, 0x11, 0x1e, 0x14, 0x12, 0x11},   // 0x52, R
    {0x0e, 0x11, 0x10, 0x0e, 0x01, 0x11, 0x0e},   // 0x53, S
    {0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},   // 0x54, T
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e},   // 0x55, U
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x0a, 0x04},   // 0x56, V
    {0x11, 0x11, 0x11, 0x15, 0x15, 0x1b, 0x11},   // 0x57, W
    {0x11, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x11},   // 0x58, X
    {0x11, 0x11, 0x0a, 0x04, 0x04, 0x04, 0x04},   // 0x59, Y
    {0x1f, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1f},   // 0x5a, Z
    {0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e},   // 0x5b, [
    {0x10, 0x10, 0x08, 0x04, 0x02, 0x01, 0x01},   // 0x5c, \
    {0x0e, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0e},   // 0x5d, ]
    {0x04, 0x0a, 0x11, 0x00, 0x00, 0x00, 0x00},   // 0x5e, ^
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f},   // 0x5f, _

    {0x04, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00},   // 0x60, `
    {0x00, 0x0e, 0x01, 0x0d, 0x13, 0x13, 0x0d},   // 0x61, a
    {0x10, 0x10, 0x10, 0x1c, 0x12, 0x12, 0x1c},   // 0x62, b
    {0x00, 0x00, 0x00, 0x0e, 0x10, 0x10, 0x0e},   // 0x63, c
    {0x01, 0x01, 0x01, 0x07, 0x09, 0x09, 0x07},   // 0x64, d
    {0x00, 0x00, 0x0e, 0x11, 0x1f, 0x10, 0x0f},   // 0x65, e
    {0x06, 0x09, 0x08, 0x1c, 0x08, 0x08, 0x08},   // 0x66, f
    {0x0e, 0x11, 0x13, 0x0d, 0x01, 0x01, 0x0e},   // 0x67, g
    {0x10, 0x10, 0x10, 0x16, 0x19, 0x11, 0x11},   // 0x68, h
    {0x00, 0x04, 0x00, 0x0c, 0x04, 0x04, 0x0e},   // 0x69, i
    {0x02, 0x00, 0x06, 0x02, 0x02, 0x12, 0x0c},   // 0x6a, j
    {0x10, 0x10, 0x12, 0x14, 0x18, 0x14, 0x12},   // 0x6b, k
    {0x0c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},   // 0x6c, l
    {0x00, 0x00, 0x0a, 0x15, 0x15, 0x11, 0x11},   // 0x6d, m
    {0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11},   // 0x6e, n
    {0x00, 0x00, 0x0e, 0x11, 0x11, 0x11, 0x0e},   // 0x6f, o
    {0x00, 0x1c, 0x12, 0x12, 0x1c, 0x10, 0x10},   // 0x70, p
    {0x00, 0x07, 0x09, 0x09, 0x07, 0x01, 0x01},   // 0x71, q
    {0x00, 0x00, 0x16, 0x19, 0x10, 0x10, 0x10},   // 0x72, r
    {0x00, 0x00, 0x0f, 0x10, 0x0e, 0x01, 0x1e},   // 0x73, s
    {0x08, 0x08, 0x1c, 0x08, 0x08, 0x09, 0x06},   // 0x74, t
    {0x00, 0x00, 0x11, 0x11, 0x11, 0x13, 0x0d},   // 0x75, u
    {0x00, 0x00, 0x11, 0x11, 0x11, 0x0a, 0x04},   // 0x76, v
    {0x00, 0x00, 0x11, 0x11, 0x15, 0x15, 0x0a},   // 0x77, w
    {0x00, 0x00, 0x11, 0x0a, 0x04, 0x0a, 0x11},   // 0x78, x
    {0x00, 0x11, 0x11, 0x0f, 0x01, 0x11, 0x0e},   // 0x79, y
    {0x00, 0x00, 0x1f, 0x02, 0x04, 0x08, 0x1f},   // 0x7a, z
    {0x06, 0x08, 0x08, 0x10, 0x08, 0x08, 0x06},   // 0x7b, {
    {0x04, 0x04, 0x04, 0x00, 0x04, 0x04, 0x04},   // 0x7c, |
    {0x0c, 0x02, 0x02, 0x01, 0x02, 0x02, 0x0c},   // 0x7d, }
    {0x08, 0x15, 0x02, 0x00, 0x00, 0x00, 0x00},   // 0x7e, ~
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // 0x7f, DEL
};



// Pre-declare ISR callback functions
void DoLedMachine(void);
void CheckBattery(void);

// Create IntervalTimer objects
TinyGPS gps;
char szInfo[64];
IntervalTimer myTimer;		// 3 for the Core
//Timer batteryCheckTimer(5000, CheckBattery);


double fuel_level;
FuelGauge fuel_gauge;

void ReadFontToArray( byte* data_buffer, char data)
{
    int idx = (int)data-(int)CHAR_OFFSET;

    if(idx < 0 || idx >= CHAR_COUNT)
    {
        //Serial.print("unprintable");
        idx = CHAR_COUNT - 1;
    }


    ReadFlashToArray( data_buffer, CHAR_HEIGHT, (int)(FontData[idx]));
}


byte ReadFlashByte( int address)
{
    return pgm_read_byte_near( address );
}

void ReadFlashToArray( byte *buffer, int count, int address)
{
    int index = 0;

    while(index < count)
    {
        buffer[index] = ReadFlashByte(address + index);
        index++;
    }
}


void CopyStringToFrameBuffer( char *stringBuffer, int x_offset, int y_offset)
{
 char *current = stringBuffer;
 int char_offset = x_offset;
 if(current != 0)
 {
   do
   {

     if(PeakCharWidth( *current ) + char_offset > 0 && char_offset < LED_ROW_BIT_COUNT)
     {

     ReadFontToArray( FontBuffer, *current);
     //ReadFlashToArray( FontBuffer, CHAR_HEIGHT, (int)(FontData[(byte)*current-CHAR_OFFSET]));
     char_offset += CopyCharToFrameBuffer( FontBuffer,CHAR_WIDTH, CHAR_HEIGHT, char_offset, y_offset * LED_ROW_ENABLE_COUNT);
     //char_offset += CopyCharToFrameBuffer( CHAR_DATA(*current),CHAR_WIDTH, CHAR_HEIGHT, char_offset, y_offset * LED_ROW_ENABLE_COUNT);
     //current++;
     }
 else
     {
       char_offset += PeakCharWidth( *current );
     }

   }while(*current++ != '\0');
 }
}

byte PeakCharWidth( char data )
{
  return 5;
}

byte CopyCharToFrameBuffer( byte *charBuffer, byte charWidth, byte charHeight, int x_offset, int y_offset )
{
  //x_offset is in pixels
  int byte_offset = x_offset / 8;
  int bit_offset = (x_offset % 8);
  byte retval = 0;

  //for now copy the bits.
  for(int rowIdx = 0; rowIdx < charHeight; rowIdx++)
  {
 int dstByte = byte_offset;
 int dstRow = rowIdx + y_offset;
 byte srcMask = 0x10; //bit 5
 byte dstMask = 0x80 >> bit_offset;

 while(srcMask > 0)
 {
   if( dstByte >= 0 && dstByte < LED_ROW_BYTE_COUNT && dstRow >= 0 && dstRow < LED_ROW_ENABLE_COUNT*2)
   {

     if(charBuffer[rowIdx] & srcMask)
     {
       //set our current bit
       frameBuffer[dstRow][dstByte] |= dstMask;
     }
     else
     {
       //clear our current bit.
       frameBuffer[dstRow][dstByte] &= ~dstMask;
     }
   }
   dstMask = dstMask >> 1;
   if(dstMask == 0)
   {
     dstMask = 0x80;
     dstByte++;
   }

   srcMask = srcMask >> 1;
   retval++;
 }
}
return 5;
}


void setup(void) {

       //turn on our serial logging at 115200
   //Serial.begin(115200);
   //Serial.println("Setup ++");

   Serial1.begin(9600);

   currentRow = 0;
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


   test = 0;
   //all power pins out and off
   for(int rowIdx = 0; rowIdx <LED_ROW_ENABLE_COUNT; rowIdx++)
   {
     pinMode( LED_ROW_ENABLE[rowIdx], OUTPUT);
     digitalWrite (LED_ROW_ENABLE[rowIdx], LOW);

     for(int dataIdx = 0; dataIdx < LED_ROW_BYTE_COUNT*2; dataIdx++)
     {
   frameBuffer[rowIdx][dataIdx] = 0;
     }

   }

   //CopyStringToFrameBuffer( "Welcome To The ", 1, 0 );
   CopyStringToFrameBuffer( "   LETS PLAY", 0, 1 );


  pinMode(DEBUG_LED, OUTPUT);		// Configure LED pins as OUTPUTs


  // AUTO allocate blinkLED to run every 500ms using hmSec timescale (1000 * 0.5ms period)
  // On Core the first allocated timer is TIMER2.  On Photon, it is TIMER3.
  myTimer.begin(DoLedMachine, TIMER_DURATION, uSec);

    Spark.publish("Started");

    //Particle.variable("fuel_level", &fuel_level, DOUBLE);

    //CheckBattery();
    //batteryCheckTimer.start();

}


const int TOP_OFFSET_START = 71;
int top_offset = -1000;

const int BTM_OFFSET_START = 71;
int btm_offset = -1000;

int TopBufferLength = -1;
int BtmBufferLength = -1;
char LastChars[4];


void loop(void) {

    //int idx;
    //char c;


    unsigned long currentTicks = millis();

    if( currentTicks - prevTicks >= 300000)
    {
        //CheckBattery();
        if(top_offset > 0 && TopBufferLength > 0)
        top_offset--;


     //if(top_offset < -(TopBufferLength)*5)
     //{
       //reset the message
       //top_offset = TOP_OFFSET_START;

       //Serial.println();
       //DoRequest();


       //TopBufferLength = -1;
       //TopBuffer[0] = 0;

     //}

     CopyStringToFrameBuffer( TopBuffer, top_offset, 0 );

     if(BtmBufferLength > 0)
       btm_offset--;
     //btm_offset--;


     if(btm_offset < -(BtmBufferLength)*5)
     {
       //reset the message
       btm_offset = BTM_OFFSET_START;
       top_offset = TOP_OFFSET_START;

       BtmBufferLength = -1;
       BtmBuffer[0] = 0;

       TopBufferLength = -1;
       TopBuffer[0] = 0;

     }
     CopyStringToFrameBuffer( BtmBuffer, btm_offset, 1 );

     fDebugState = !fDebugState;

     if(fDebugState)
     {
       digitalWrite( DEBUG_LED, HIGH);
     }
     else
     {
         digitalWrite( DEBUG_LED, LOW);

     }

        {
        bool isValidGPS = false;

        for (unsigned long start = millis(); millis() - start < 1000;){
            // Check GPS data is available
            while (Serial1.available()){
                char c = Serial1.read();

                // parse GPS data
                if (gps.encode(c))
                    isValidGPS = true;
            }
        }

        // If we have a valid GPS location then publish it
        if (isValidGPS){
            float lat, lon;
            unsigned long age;

            gps.f_get_position(&lat, &lon, &age);

            sprintf(szInfo, "%.6f,%.6f", (lat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lat), (lon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lon));
        }
        else{
            // Not a vlid GPS location, jsut pass 0.0,0.0
            // This is not correct because 0.0,0.0 is a valid GPS location, we have to pass a invalid GPS location
            // and check it at the client side
            sprintf(szInfo, "0.0,0.0");
        }

        Spark.publish("gpsloc", szInfo);
   }

    CheckBattery();


     prevTicks = currentTicks;
   }



   // System.sleep(60);

}

void CheckBattery(void)
{
    char message[32];

    fuel_level = fuel_gauge.getSoC();
    sprintf(message, "%f - %f", fuel_level, fuel_gauge.getVCell());
    Particle.publish("BatteryState", message);

}


 void DoLedMachine()
 {


   switch(currentState)
   {
     default:
     case LED_STATE_INIT:
       currentState = LED_STATE_RESET_ROW_DATA;
     break;


     case LED_STATE_RESET_ROW_DATA:
       bitIndex = 0;
       byteIndex = -1;
     case LED_STATE_LOAD_BYTE_DATA:
       byteIndex++;
       currentUpperData = frameBuffer[currentRow][byteIndex];
       currentLowerData = frameBuffer[currentRow+LED_ROW_ENABLE_COUNT][byteIndex];
       bitMask = 0x80;
     case LED_STATE_PREPARE_BIT:

       SR_TOP_CLK_RESET;
       if( currentUpperData & bitMask)
         SR_TOP_DAT_HIGH;
       else
         SR_TOP_DAT_LOW;

       SR_BTM_CLK_RESET;
       if( currentLowerData & bitMask)
         SR_BTM_DAT_HIGH;
       else
         SR_BTM_DAT_LOW;
       currentState = LED_STATE_LATCH_BIT;
     break;


     case LED_STATE_LATCH_BIT:

       SR_TOP_CLK_LATCH;

       SR_BTM_CLK_LATCH;

       bitIndex++;
       if(bitIndex >= LED_ROW_BIT_COUNT)
       {

     //turn off the shift registers outputs;
         SR_OUTPUT_DISABLE;
         currentState = LED_STATE_END_ROW;
       }
       else
       {

         //still have shit to send.
         bitMask = bitMask >> 1;


         //if we just shifted the mask off the end of the byte, then we need to move to the next byte.
         if ( bitMask == 0)
         {
           currentState = LED_STATE_LOAD_BYTE_DATA;
         }
         else
         {
           currentState = LED_STATE_PREPARE_BIT;

         }
       }


     break;
     case LED_STATE_END_ROW:
             //we need to set all the powers to off,
         for(int x = 0; x <LED_ROW_ENABLE_COUNT; x++)
         {
           digitalWrite (LED_ROW_ENABLE[x], LOW);
         }

         //latch the next bits
         SR_LATCH_ENABLE;

       currentState = LED_STATE_NEXT_ROW;
     break;


     case LED_STATE_NEXT_ROW:
       //turn on the power to this row
       digitalWrite(LED_ROW_ENABLE[currentRow], HIGH);


       //latch the shift registers
       SR_LATCH_RESET;


       //turn on the outputs for the shift registers
       SR_OUTPUT_ENABLE;


       //setup our vars to shift in the next row
       currentRow += 2;
       if( currentRow >= LED_ROW_ENABLE_COUNT)
         currentRow -= LED_ROW_ENABLE_COUNT;


       currentState = LED_STATE_RESET_ROW_DATA;
     break;
  }
 }
