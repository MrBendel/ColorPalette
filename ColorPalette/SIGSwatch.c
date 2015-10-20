//
//  SIGSwatch.c
//  Signals
//
//  Created by Andrew Poes on 7/9/15.
//  Copyright (c) 2015 Signals. All rights reserved.
//

#include "SIGSwatch.h"
#include <math.h>

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

const double MinContrastTitleText = 3.0;
const double MinContrastBodyText = 4.5;

SIGSwatch SIGSwatchMake(unsigned char red, unsigned char green, unsigned char blue, int population) {
    return (SIGSwatch){ .red = red, .green = green, .blue = blue, .population = population };
}

SIGHSL SIGSwatchGetHSL(SIGSwatch swatch) {
    double rf = swatch.red / 255.0;
    double gf = swatch.green / 255.0;
    double bf = swatch.blue / 255.0;
    
    double mx = max(rf, max(gf, bf));
    double mn = min(rf, min(gf, bf));
    
    double deltaMxMn = mx - mn;
    
    double h,s,
    l = (mx+mn) / 2.0;
    
    if (mx == mn) {
        // Monochromatic
        h = 0;
        s = 0;
    }
    else {
        if (mx == rf) {
            h = fmod((gf - bf) / deltaMxMn, 6.0);
        } else if (mx == gf) {
            h = ((bf - rf) / deltaMxMn) + 2;
        } else {
            h = ((rf - gf) / deltaMxMn) + 4;
        }
        s =  deltaMxMn / (1.0 - fabs(2.0 * l - 1.0));
    }
    
    return (SIGHSL){ .hue = fmod((h * 60), 360), .saturation = s, .lightness = l };
}

int SIGSwatchGetHashValue(SIGSwatch swatch) {
    return 31 * swatch.red + 11 * swatch.green + 7 * swatch.blue + swatch.population;
}

bool SIGSwatchEqualToSwatch(SIGSwatch a, SIGSwatch b) {
    return SIGSwatchGetHashValue(a) == SIGSwatchGetHashValue(b);
}

bool SIGHSLIsBlack(SIGHSL hsl, double maxBlack) {
    return hsl.lightness < maxBlack;
}

bool SIGHSLIsWhite(SIGHSL hsl, double minWhite) {
    return hsl.lightness > minWhite;
}

bool SIGHSLIsNearRedILine(SIGHSL hsl) {
    return hsl.hue >= 10 && hsl.hue <= 37 && hsl.saturation <= 0.82;
}
