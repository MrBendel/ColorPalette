//
//  SIGColor.h
//  Signals
//
//  Created by Andrew Poes on 7/9/15.
//  Copyright (c) 2015 Signals. All rights reserved.
//

#ifndef __Signals__SIGColor__
#define __Signals__SIGColor__

#include <stdio.h>

typedef enum SIGColorComponent {
    SIGColorComponentRed = 0,
    SIGColorComponentGreen,
    SIGColorComponentBlue,
    SIGColorComponentAlpha
} SIGColorComponent;

typedef uint32_t SIGColor;

extern uint32_t SIGColorGetRed32(SIGColor color);
extern uint32_t SIGColorGetGreen32(SIGColor color);
extern uint32_t SIGColorGetBlue32(SIGColor color);
extern uint32_t SIGColorGetAlpha32(SIGColor color);
extern void SIGColorGetComponents32(SIGColor color, uint32_t *r, uint32_t *g, uint32_t *b, uint32_t *a);
extern SIGColor SIGColorFromRGBA32(uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha);
extern SIGColor SIGColorFromRGBA8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
extern char* SIGColorGetDescription(SIGColor color);
extern int SIGColorCompare( const void* a, const void* b);
extern int SIGColorCompareRed( const void* a, const void* b);
extern int SIGColorCompareGreen( const void* a, const void* b);
extern int SIGColorCompareBlue( const void* a, const void* b);

#endif /* defined(__Signals__SIGColor__) */
