//
//  SIGVBox.h
//  Signals
//
//  Created by Andrew Poes on 7/15/15.
//  Copyright (c) 2015 Signals. All rights reserved.
//

#ifndef __Signals__SIGVBox__
#define __Signals__SIGVBox__

#include <stdio.h>
#include "SIGSwatch.h"
#include "SIGColorHistogram.h"

typedef uint32_t SIGVboxStatusCode;
static SIGVboxStatusCode kSIGVboxStatusCodeInitializationFailure    = 0;
static SIGVboxStatusCode kSIGVboxStatusCodeInitializationSuccess    = 1;

// Represents a tightly fitting box around a color space.

typedef struct SIGVBox {
    SIGColor *mColors;
    int mNumColors;
    SIGColorHistogram mHisto;
    uint32_t mMinRed;
    uint32_t mMaxRed;
    uint32_t mMinGreen;
    uint32_t mMaxGreen;
    uint32_t mMinBlue;
    uint32_t mMaxBlue;
} SIGVBox;

extern SIGVBox SIGVboxMake(SIGColor *colors, int totalColors, SIGColorHistogram histo);
extern void SIGVboxInit(SIGVBox *vbox, SIGColor *colors, int totalColors, SIGColorHistogram histo);
extern void SIGVBoxFitBox(SIGVBox *vbox);
extern uint32_t SIGVBoxCanSplit(SIGVBox vbox);
extern uint32_t SIGVBoxGetColorCount(SIGVBox vbox);
extern SIGVboxStatusCode SIGVBoxSplitBox(SIGVBox *vbox, SIGVBox *newVbox);
extern SIGColorComponent SIGVboxGetLongestColorDimension(SIGVBox vbox);
extern uint32_t SIGVBoxFindSplitPoint(SIGVBox *vbox);
extern SIGSwatch SIGVboxGetAverageColor(SIGVBox vbox);
extern void SIGVboxDestroy(SIGVBox *vbox);
extern int SIGVboxCompareVolume( const void* a, const void* b);

#endif /* defined(__Signals__SIGVBox__) */
