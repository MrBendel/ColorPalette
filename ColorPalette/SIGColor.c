//
//  SIGColor.c
//  Signals
//
//  Created by Andrew Poes on 7/9/15.
//  Copyright (c) 2015 Signals. All rights reserved.
//

#include "SIGColor.h"

uint32_t SIGColorGetRed32(SIGColor color) {
    return color >> 16 & 0xFF;
}

uint32_t SIGColorGetGreen32(SIGColor color) {
    return color >> 8 & 0xFF;
}

uint32_t SIGColorGetBlue32(SIGColor color) {
    return color >> 0 & 0xFF;
}

uint32_t SIGColorGetAlpha32(SIGColor color) {
    return color >> 24 & 0xFF;
}

void SIGColorGetComponents32(SIGColor color, uint32_t *r, uint32_t *g, uint32_t *b, uint32_t *a) {
    *r = SIGColorGetRed32(color);
    *g = SIGColorGetGreen32(color);
    *b = SIGColorGetBlue32(color);
    *a = SIGColorGetAlpha32(color);
}

SIGColor SIGColorFromRGBA32(uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha) {
    return ((red & 0xFF) << 16) + ((green & 0xFF) << 8) + ((blue & 0xFF) << 0) + ((alpha & 0xFF) << 24);
}

SIGColor SIGColorFromRGBA8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    return ((red & 0xFF) << 16) + ((green & 0xFF) << 8) + ((blue & 0xFF) << 0) + ((alpha & 0xFF) << 24);
}

extern char* SIGColorGetDescription(SIGColor color) {
    char *str;
    sprintf(str, "%d", color);
    return str;
}

int __ComponentCompare(int a, int b) {
    if ( a == b ) return 0;
    else if ( a < b ) return -1;
    else return 1;
}

int SIGColorCompare( const void* a, const void* b) {
    SIGColor color_a = * ( (SIGColor*) a );
    SIGColor color_b = * ( (SIGColor*) b );
    return __ComponentCompare(color_a, color_b);
}

int SIGColorCompareRed( const void* a, const void* b) {
    SIGColor color_a = * ( (SIGColor*) a );
    SIGColor color_b = * ( (SIGColor*) b );
    return __ComponentCompare(SIGColorGetRed32(color_a), SIGColorGetRed32(color_b));
}

int SIGColorCompareGreen( const void* a, const void* b) {
    SIGColor color_a = * ( (SIGColor*) a );
    SIGColor color_b = * ( (SIGColor*) b );
    return __ComponentCompare(SIGColorGetGreen32(color_a), SIGColorGetGreen32(color_b));
}

int SIGColorCompareBlue( const void* a, const void* b) {
    SIGColor color_a = * ( (SIGColor*) a );
    SIGColor color_b = * ( (SIGColor*) b );
    return __ComponentCompare(SIGColorGetBlue32(color_a), SIGColorGetBlue32(color_b));
}

