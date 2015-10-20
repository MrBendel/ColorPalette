//
//  SIGColorCutQuantizer.h
//  Signals
//
//  Created by Andrew Poes on 7/9/15.
//  Copyright (c) 2015 Signals. All rights reserved.
//

#ifndef __Signals__SIGColorCutQuantizer__
#define __Signals__SIGColorCutQuantizer__

#include <stdio.h>
#include "SIGColor.h"
#include "SIGSwatch.h"
#include "SIGColorHistogram.h"
#include "SIGVBox.h"

extern const double kSIGColorCutBlackMaxLightness;
extern const double kSIGColorCutWhiteMinLightness;

typedef struct SIGColorCutQuantizer {
    SIGColor *mColors;
    int mNumColors;
    SIGColorHistogram mHisto;
    SIGSwatch *mQuantizedColors;
    int mTotalQuantizedColors;
} SIGColorCutQuantizer;

extern SIGColorCutQuantizer SIGColorCutQuantizerMake(SIGColor *colors, int numColors, int maxColors);
extern SIGSwatch* SIGColorCutQuantizerQuantizePixels(SIGColorCutQuantizer quantizer, int maxColors, int *totalSwatches);
extern void SIGColorCutQuantizerSplitBoxes(SIGVBox *boxes, int *boxesLength, int maxColors);
extern SIGSwatch* SIGColorCutQuantizerGenerateAverageColors(SIGVBox *boxes, int numBoxes, int *totalSwatches);
extern bool SIGColorShouldIgnore(SIGColor color);
extern bool SIGSwatchShouldIgnore(SIGSwatch swatch);

#endif /* defined(__Signals__SIGColorCutQuantizer__) */
