//
//  SIGColorHistogram.h
//  Signals
//
//  Created by Andrew Poes on 7/9/15.
//  Copyright (c) 2015 Signals. All rights reserved.
//

#ifndef __Signals__SIGColorHistogram__
#define __Signals__SIGColorHistogram__

#define SIGBITS 5
#define RSHIFT (8 - SIGBITS)
#define HISTOSIZE 1 << (3 * SIGBITS)

#include <stdio.h>
#include "SIGColor.h"
#include "uthash.h"

typedef struct SIGColorHash {
    SIGColor color;
    int population;
    UT_hash_handle hh;
} SIGColorHash;

typedef struct SIGColorHistogram {
    SIGColor *mColors;
    int mNumColors;
    int *mColorCounts;
    SIGColorHash *mColorHash;
} SIGColorHistogram;

extern SIGColorHistogram SIGColorHistogramMakeFromColorArray(SIGColor* pixels, int pixelCount);
extern int SIGColorHistogramCountDistinctColors(SIGColor* pixels, int pixelCount);
extern void SIGColorHistogramCountColorFrequencies(SIGColorHistogram* histo, SIGColor* pixels, int pixelCount);
extern void SIGColorHistogramDestroy(SIGColorHistogram *histo);
extern int SIGColorHistogramCountForColor(SIGColorHistogram histo, SIGColor color);

extern int SIGColorHistogramGetColorIndex(int red, int green, int blue);
extern int SIGColorHistogramGetColorIndexForColor(SIGColor color);

#endif /* defined(__Signals__SIGColorHistogram__) */
