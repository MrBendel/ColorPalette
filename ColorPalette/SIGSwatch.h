//
//  SIGSwatch.h
//  Signals
//
//  Created by Andrew Poes on 7/9/15.
//  Copyright (c) 2015 Signals. All rights reserved.
//

#ifndef __Signals__SIGSwatch__
#define __Signals__SIGSwatch__

#include <stdio.h>
#include <stdbool.h>

extern const double MinContrastTitleText;
extern const double MinContrastBodyText;

/**
 * Swatch
 *     red [0...255]
 *     green [0...255]
 *     blue [0...255]
 */
typedef struct SIGSwatch {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    int population;
} SIGSwatch;

extern SIGSwatch SIGSwatchMake(unsigned char red, unsigned char green, unsigned char blue, int population);
extern int SIGSwatchGetHashValue(SIGSwatch swatch);
extern bool SIGSwatchEqualToSwatch(SIGSwatch a, SIGSwatch b);

/**
 * Swatch's HSL values.
 *     hsv[0] is Hue [0 .. 360)
 *     hsv[1] is Saturation [0...1]
 *     hsv[2] is Lightness [0...1]
 */
typedef struct SIGHSL {
    double hue;
    double saturation;
    double lightness;
} SIGHSL;

extern SIGHSL SIGSwatchGetHSL(SIGSwatch swatch);
extern bool SIGHSLIsBlack(SIGHSL hsl, double maxBlack);
extern bool SIGHSLIsWhite(SIGHSL hsl, double minWhite);
extern bool SIGHSLIsNearRedILine(SIGHSL hsl);

#endif /* defined(__Signals__SIGSwatch__) */
