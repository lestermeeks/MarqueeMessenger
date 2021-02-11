#ifndef _GOLDENTEEFRAMEBUFFER_H
#define _GOLDENTEEFRAMEBUFFER_H

#include "application.h"

// FONT STUFF>
#define CHAR_WIDTH 5 
#define CHAR_HEIGHT 7 
#define CHAR_COUNT 96

typedef struct _font_char_desc_
{
    byte width;                     //the actual rendered width for var
    byte bitmap[CHAR_WIDTH];        //the cols of the bitmap data... lowest CHAR_HEIGHT bits
} FONT_CHAR_DESC;

class GoldenTeeFont
{
    public:
        GoldenTeeFont();
        ~GoldenTeeFont();
};


#define BUFFER_PIXEL_WIDTH 70
#define BUFFER_PIXEL_HEIGHT 14

class GoldenTeeFrameBuffer
{
    public:
	    GoldenTeeFrameBuffer();
	    ~GoldenTeeFrameBuffer();

        int GetPixelWidth();
        int GetPixelHeight();
        bool GetPixel(int x, int y);
        bool GetPixelFast(int x, int y);
        void SetPixel(int x, int y, bool state, int x_motion);
        void Clear();

        //void CopyStringToFrameBuffer( char *stringBuffer, int x_offset, int y_offset);
        void CopyStringToFrameBuffer( char *stringBuffer, int x_offset, int y_offset, int scale, int motion);
        //int MeasureString(char *buffer);
        int MeasureStringVar(char *buffer, int scale);
    private:
        int pixelDataWidth = BUFFER_PIXEL_WIDTH, pixelDataHeight = BUFFER_PIXEL_HEIGHT;
        bool pixelData[BUFFER_PIXEL_WIDTH][BUFFER_PIXEL_HEIGHT];
        
        //byte PeakCharWidth( char data );
        //void CopyCharToFrameBuffer( byte *charBuffer, byte charWidth, byte charHeight, int x_offset, int y_offset );
        //byte CopyCharToFrameBufferVar(FONT_CHAR_DESC *charDesc, int x_offset, int y_offset );
        byte CopyCharToFrameBuffer(FONT_CHAR_DESC *charDesc, int x_offset, int y_offset, int x_scale, int y_scale, int motion);
};



#endif