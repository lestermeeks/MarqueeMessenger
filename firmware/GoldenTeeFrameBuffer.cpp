#include "GoldenTeeFrameBuffer.h"


#define CHAR_DATA(a) (byte*)( FontData[(int)(a - 0x20)] ) 
#define CHAR_DESC(a) (FONT_CHAR_DESC*)( FontDesc + (int)(a - 0x20) ) 





const FONT_CHAR_DESC FontDesc[CHAR_COUNT] = {
    { 3, { 0x00, 0x00, 0x00, 0x00, 0x00 }},  //space
	{ 3, { 0x00, 0x5F, 0x00, 0x00, 0x00 }},// !
	{ 5, { 0x00, 0x07, 0x00, 0x07, 0x00 }},// "
	{ 5, { 0x14, 0x7F, 0x14, 0x7F, 0x14 }},// #
	{ 5, { 0x24, 0x2A, 0x7F, 0x2A, 0x12 }},// $
	{ 5, { 0x23, 0x13, 0x08, 0x64, 0x62 }},// %
	{ 5, { 0x36, 0x49, 0x55, 0x22, 0x50 }},// &
	{ 3, { 0x00, 0x05, 0x03, 0x00, 0x00 }},// '
	{ 3, { 0x1C, 0x22, 0x41, 0x00, 0x00 }},// (
	{ 3, { 0x41, 0x22, 0x1C, 0x00, 0x00 }},// )
	{ 5, { 0x08, 0x2A, 0x1C, 0x2A, 0x08 }},// *
	{ 5, { 0x08, 0x08, 0x3E, 0x08, 0x08 }},// +
	{ 3, { 0x00, 0x50, 0x30, 0x00, 0x00 }},// ,
	{ 4, { 0x00, 0x08, 0x08, 0x00, 0x00 }},// -
	{ 4, { 0x00, 0x60, 0x60, 0x00, 0x00 }},// .
	{ 5, { 0x20, 0x10, 0x08, 0x04, 0x02 }},// /
	{ 5, { 0x3E, 0x51, 0x49, 0x45, 0x3E }},// 0
	{ 5, { 0x00, 0x42, 0x7F, 0x40, 0x00 }},// 1
	{ 5, { 0x42, 0x61, 0x51, 0x49, 0x46 }},// 2
	{ 5, { 0x21, 0x41, 0x45, 0x4B, 0x31 }},// 3
	{ 5, { 0x18, 0x14, 0x12, 0x7F, 0x10 }},// 4
	{ 5, { 0x27, 0x45, 0x45, 0x45, 0x39 }},// 5
	{ 5, { 0x3C, 0x4A, 0x49, 0x49, 0x30 }},// 6
	{ 5, { 0x01, 0x71, 0x09, 0x05, 0x03 }},// 7
	{ 5, { 0x36, 0x49, 0x49, 0x49, 0x36 }},// 8
	{ 5, { 0x06, 0x49, 0x49, 0x29, 0x1E }},// 9
	{ 4, { 0x00, 0x36, 0x36, 0x00, 0x00 }},// :
	{ 4, { 0x00, 0x56, 0x36, 0x00, 0x00 }},// ;
	{ 5, { 0x00, 0x08, 0x14, 0x22, 0x41 }},// <
	{ 5, { 0x14, 0x14, 0x14, 0x14, 0x14 }},// =
	{ 5, { 0x41, 0x22, 0x14, 0x08, 0x00 }},// >
	{ 5, { 0x02, 0x01, 0x51, 0x09, 0x06 }},// ?
	{ 5, { 0x32, 0x49, 0x79, 0x41, 0x3E }},// @
	{ 5, { 0x7E, 0x11, 0x11, 0x11, 0x7E }},// A
	{ 5, { 0x7F, 0x49, 0x49, 0x49, 0x36 }},// B
	{ 5, { 0x3E, 0x41, 0x41, 0x41, 0x22 }},// C
	{ 5, { 0x7F, 0x41, 0x41, 0x22, 0x1C }},// D
	{ 5, { 0x7F, 0x49, 0x49, 0x49, 0x41 }},// E
	{ 4, { 0x7F, 0x09, 0x09, 0x01, 0x00 }},// F
	{ 5, { 0x3E, 0x41, 0x41, 0x51, 0x32 }},// G
	{ 5, { 0x7F, 0x08, 0x08, 0x08, 0x7F }},// H
	{ 3, { 0x41, 0x7F, 0x41, 0x00, 0x00 }},// I
	{ 5, { 0x20, 0x40, 0x41, 0x3F, 0x01 }},// J
	{ 5, { 0x7F, 0x08, 0x14, 0x22, 0x41 }},// K
	{ 5, { 0x7F, 0x40, 0x40, 0x40, 0x40 }},// L
	{ 5, { 0x7F, 0x02, 0x04, 0x02, 0x7F }},// M
	{ 5, { 0x7F, 0x04, 0x08, 0x10, 0x7F }},// N
	{ 5, { 0x3E, 0x41, 0x41, 0x41, 0x3E }},// O
	{ 5, { 0x7F, 0x09, 0x09, 0x09, 0x06 }},// P
	{ 5, { 0x3E, 0x41, 0x51, 0x21, 0x5E }},// Q
	{ 5, { 0x7F, 0x09, 0x19, 0x29, 0x46 }},// R
	{ 5, { 0x46, 0x49, 0x49, 0x49, 0x31 }},// S
	{ 5, { 0x01, 0x01, 0x7F, 0x01, 0x01 }},// T
	{ 5, { 0x3F, 0x40, 0x40, 0x40, 0x3F }},// U
	{ 5, { 0x1F, 0x20, 0x40, 0x20, 0x1F }},// V
	{ 5, { 0x7F, 0x20, 0x18, 0x20, 0x7F }},// W
	{ 5, { 0x63, 0x14, 0x08, 0x14, 0x63 }},// X
	{ 5, { 0x03, 0x04, 0x78, 0x04, 0x03 }},// Y
	{ 5, { 0x61, 0x51, 0x49, 0x45, 0x43 }},// Z
	{ 4, { 0x00, 0x7F, 0x41, 0x41, 0x00 }},// [
	{ 5, { 0x02, 0x04, 0x08, 0x10, 0x20 }},// "\"
	{ 4, { 0x41, 0x41, 0x7F, 0x00, 0x00 }},// ]
	{ 5, { 0x04, 0x02, 0x01, 0x02, 0x04 }},// ^
	{ 5, { 0x40, 0x40, 0x40, 0x40, 0x40 }},// _
	{ 3, { 0x01, 0x02, 0x04, 0x00, 0x00 }},// `
	{ 4, { 0x20, 0x54, 0x54, 0x78, 0x00 }},// a
	{ 5, { 0x7F, 0x48, 0x44, 0x44, 0x38 }},// b
	{ 4, { 0x38, 0x44, 0x44, 0x08, 0x00 }},// c
	{ 4, { 0x38, 0x44, 0x44, 0x7F, 0x00 }},// d
	{ 4, { 0x38, 0x54, 0x54, 0x18, 0x00 }},// e
	{ 4, { 0x08, 0x7E, 0x09, 0x01, 0x00 }},// f
	{ 4, { 0x08, 0x54, 0x54, 0x3C, 0x00 }},// g
	{ 4, { 0x7F, 0x08, 0x04, 0x78, 0x00 }},// h
	{ 3, { 0x00, 0x7D, 0x00, 0x00, 0x00 }},// i
	{ 5, { 0x20, 0x40, 0x44, 0x3D, 0x00 }},// j
	{ 5, { 0x00, 0x7F, 0x10, 0x28, 0x44 }},// k
	{ 3, { 0x41, 0x7F, 0x40, 0x00, 0x00 }},// l
	{ 5, { 0x7C, 0x04, 0x18, 0x04, 0x78 }},// m
	{ 4, { 0x7C, 0x08, 0x04, 0x78, 0x00 }},// n
	{ 4, { 0x38, 0x44, 0x44, 0x38, 0x00 }},// o
	{ 4, { 0x7C, 0x14, 0x14, 0x08, 0x00 }},// p
	{ 5, { 0x08, 0x14, 0x14, 0x18, 0x7C }},// q
	{ 4, { 0x7C, 0x08, 0x04, 0x04, 0x00 }},// r
	{ 4, { 0x48, 0x54, 0x54, 0x24, 0x00 }},// s
	{ 3, { 0x04, 0x3F, 0x44, 0x40, 0x20 }},// t
	{ 4, { 0x3C, 0x40, 0x40, 0x3C, 0x00 }},// u
	{ 5, { 0x1C, 0x20, 0x40, 0x20, 0x1C }},// v
	{ 5, { 0x3C, 0x40, 0x30, 0x40, 0x3C }},// w
	{ 5, { 0x44, 0x28, 0x10, 0x28, 0x44 }},// x
	{ 4, { 0x0C, 0x50, 0x50, 0x3C, 0x00 }},// y
	{ 5, { 0x44, 0x64, 0x54, 0x4C, 0x44 }},// z
	{ 4, { 0x00, 0x08, 0x36, 0x41, 0x00 }},// {
	{ 3, { 0x00, 0x00, 0x7F, 0x00, 0x00 }},// |
	{ 4, { 0x00, 0x41, 0x36, 0x08, 0x00 }},// }
	{ 5, { 0x08, 0x08, 0x2A, 0x1C, 0x08 }},// ->
	{ 5, { 0x08, 0x1C, 0x2A, 0x08, 0x08 }} // <-
};
/*
const byte FontData[CHAR_COUNT][CHAR_HEIGHT] = { 

    
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
    {0x10, 0x10, 0x08, 0x04, 0x02, 0x01, 0x01},   // 0x5c, {BACKSLASH} 
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
*/
GoldenTeeFrameBuffer::GoldenTeeFrameBuffer()
{
    Clear();
}
GoldenTeeFrameBuffer::~GoldenTeeFrameBuffer()
{
    
    //delete[] pixelData;
}

int GoldenTeeFrameBuffer::GetPixelWidth()
{
    return pixelDataWidth;
}

int GoldenTeeFrameBuffer::GetPixelHeight()
{
    return pixelDataHeight;
}

bool GoldenTeeFrameBuffer::GetPixelFast(int x, int y)
{
    return pixelData[x][y];
}


bool GoldenTeeFrameBuffer::GetPixel(int x, int y)
{
    if(x >= 0 && x < pixelDataWidth && y >= 0 && y < pixelDataHeight)
        return pixelData[x][y];
    else
        return false;
}
void GoldenTeeFrameBuffer::SetPixel(int x, int y, bool state, int x_motion)
{
    int comp_x = x;
    
    if(y >= 0 && y<7)
        comp_x += x_motion;
        
    if(comp_x >= 0 && comp_x < pixelDataWidth && y >= 0 && y < pixelDataHeight)
    {
        pixelData[comp_x][y] = state;
    }
}
    
void GoldenTeeFrameBuffer::Clear()
{
    memset(pixelData, 0, pixelDataWidth * pixelDataHeight * sizeof(bool));
}
/*
void GoldenTeeFrameBuffer::CopyStringToFrameBuffer( char *stringBuffer, int x_offset, int y_offset) 
{ 

    char *current = stringBuffer; 
    int char_offset = x_offset; 

    if(current != 0) 
    { 
        while(*current != '\0') 
        { 
            int charWidth = PeakCharWidth( *current );
            if(charWidth + char_offset > 0 && char_offset < pixelDataWidth) 
                CopyCharToFrameBuffer( CHAR_DATA(*current),CHAR_WIDTH, CHAR_HEIGHT, char_offset, y_offset);

            char_offset += PeakCharWidth( *current ); 
            current++;
        }
    } 
    
}
*/
void GoldenTeeFrameBuffer::CopyStringToFrameBuffer( char *stringBuffer, int x_offset, int y_offset, int scale, int motion) 
{ 
    char *current = stringBuffer; 
    int char_offset = x_offset; 

    if(current != 0) 
    { 
        while(*current != '\0') 
        { 
            FONT_CHAR_DESC *charDesc = CHAR_DESC(*current);

            //is this char on the screen?
            if((charDesc->width*scale) + char_offset > 0 && char_offset < pixelDataWidth) 
                CopyCharToFrameBuffer( charDesc, char_offset, y_offset, scale, scale, motion);

            char_offset += (charDesc->width*scale);
            char_offset += (scale-1);
            current++;
        }
    } 
    
}
/*
int GoldenTeeFrameBuffer::MeasureString(char* buffer)
{
    int retval = 0;
    char *current = buffer; 
    
    if(current != NULL) 
    {
        //loop til a null
        while(*current != '\0')
        { 
            retval += PeakCharWidth( *current );
            current++;
        }
    }
    
    return retval;
}
*/
int GoldenTeeFrameBuffer::MeasureStringVar(char* buffer, int scale)
{
    int retval = 0;
    char *current = buffer; 
    
    if(current != NULL) 
    {
        //loop til a null
        while(*current != '\0')
        {
            FONT_CHAR_DESC *charDesc = CHAR_DESC(*current);
             
            retval += (charDesc->width*scale);
            retval += (scale-1);
            current++;
        }
    }
    
    return retval;
}
/*
byte GoldenTeeFrameBuffer::PeakCharWidth( char data ) 
{ 
  return 5; 
} 

void GoldenTeeFrameBuffer::CopyCharToFrameBuffer( byte *charBuffer, byte charWidth, byte charHeight, int x_offset, int y_offset ) 
{
    //for now copy the bits. 
    for(int rowIdx = 0; rowIdx < charHeight; rowIdx++) 
    {
        int targetRow = rowIdx + y_offset;
        if(targetRow >= 0 && targetRow < pixelDataHeight )
        {
            for(int colIdx = 0; colIdx < charWidth; colIdx++)
            {
                int targetCol = colIdx + x_offset;
                if(targetCol >= 0 && targetCol < pixelDataWidth)
                {
                    bool value = charBuffer[rowIdx] & (0x10 >> colIdx);
                    SetPixel(targetCol, targetRow, value);
                }
                
            }
        }
    } 

}
*/

/*
byte GoldenTeeFrameBuffer::CopyCharToFrameBufferVar(FONT_CHAR_DESC *charDesc, int x_offset, int y_offset, int motion ){
        //for now copy the bits. 
    for(int colIdx = 0; colIdx < charDesc->width; colIdx++)
    {
        int targetCol = colIdx + x_offset;
        if(targetCol >= 0 && targetCol < pixelDataWidth)
        {
            for(int rowIdx = 0; rowIdx < CHAR_HEIGHT; rowIdx++) 
            {
                int targetRow = rowIdx + y_offset;
                if(targetRow >= 0 && targetRow < pixelDataHeight )
                {
                    bool value = charDesc->bitmap[colIdx] & (0x01 << rowIdx);
                    SetPixel(targetCol, targetRow, value);
                }
                
            }
        }
    }
    
    return charDesc->width;
}
*/
byte GoldenTeeFrameBuffer::CopyCharToFrameBuffer(FONT_CHAR_DESC *charDesc, int x_offset, int y_offset, int x_scale, int y_scale, int motion){
            //for now copy the bits. 
    for(int colIdx = 0; colIdx < charDesc->width; colIdx++)
    {
        for(int x_scale_count = 0; x_scale_count < x_scale; x_scale_count++)
        {
            int targetCol = x_offset + colIdx*x_scale + x_scale_count;
            if(targetCol >= 0 && targetCol < pixelDataWidth)
            {
                for(int rowIdx = 0; rowIdx < CHAR_HEIGHT; rowIdx++) 
                {
                    for(int y_scale_count = 0; y_scale_count < y_scale; y_scale_count++)
                    {
                        int targetRow = y_offset + rowIdx*y_scale + y_scale_count;
                        if(targetRow >= 0 && targetRow < pixelDataHeight )
                        {
                            bool value = charDesc->bitmap[colIdx] & (0x01 << rowIdx);
                            SetPixel(targetCol, targetRow, value, motion);
                        }
                    }
                }
            }
        }
    }
    
    return charDesc->width * x_scale;
}

GoldenTeeFont::GoldenTeeFont(){
    
}
GoldenTeeFont::~GoldenTeeFont(){
    
}