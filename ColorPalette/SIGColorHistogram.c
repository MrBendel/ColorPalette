//
//  SIGColorHistogram.c
//  Signals
//
//  Created by Andrew Poes on 7/9/15.
//  Copyright (c) 2015 Signals. All rights reserved.
//

#include "SIGColorHistogram.h"
#include <stdlib.h>

#pragma mark - Hashing 

SIGColorHash* SIGColorHistogramHashColor(SIGColorHash *hash, SIGColor color, int population) {
    struct SIGColorHash *s;
    HASH_FIND_INT(hash, &color, s);  /* id already in the hash? */
    if (s==NULL) {
        s = (struct SIGColorHash*)malloc(sizeof(struct SIGColorHash));
        s->color = color;
        HASH_ADD_INT( hash, color, s );  /* id: name of key field */
    }
    s->population = population;
    return hash;
}

struct SIGColorHash *SIGColorHistogramGetHashColor(SIGColorHash *hash, SIGColor color) {
    struct SIGColorHash *s;
    HASH_FIND_INT( hash, &color, s );  /* s: output pointer */
    return s;
}

void SIGColorHistogramDeleteAllHashes(SIGColorHash *hash) {
    struct SIGColorHash *color, *tmp;
    HASH_ITER(hh, hash, color, tmp) {
        HASH_DEL(hash,color);  /* delete it (users advances to next) */
        free(color);            /* free it */
    }
}

#pragma mark - Histograming

SIGColorHistogram SIGColorHistogramMakeFromColorArray(SIGColor* pixels, int pixelCount) {
    SIGColorHistogram histogram;
    qsort(pixels, pixelCount, sizeof(SIGColor), SIGColorCompare);
    histogram.mNumColors = SIGColorHistogramCountDistinctColors(pixels, pixelCount);
    histogram.mColors = malloc(histogram.mNumColors * sizeof(SIGColor));
    histogram.mColorCounts = calloc(histogram.mNumColors, sizeof(int));
    SIGColorHistogramCountColorFrequencies(&histogram, pixels, pixelCount);
    return histogram;
}

int SIGColorHistogramCountDistinctColors(SIGColor* pixels, int pixelCount) {
    if (pixelCount < 2) {
        // If we have less than 2 pixels we can stop here
        return pixelCount;
    }
    
    int colorCount = 1;
    SIGColor currentColor = pixels[0];
    
    for (int i = 1; i < pixelCount; ++i) {
        SIGColor color = pixels[i];
        if (color != currentColor) {
            currentColor = color;
            ++colorCount;
        }
    }
    
    return colorCount;
}

void SIGColorHistogramCountColorFrequencies(SIGColorHistogram* histo, SIGColor* pixels, int pixelCount) {
    if (pixelCount == 0) {
        return;
    }
    
    int currentColorIndex = 0;
    SIGColor currentColor = pixels[0];
    
    histo->mColors[currentColorIndex] = currentColor;
    histo->mColorCounts[currentColorIndex] = 1;
    
    if (pixelCount == 1) {
        // If we only have one pixel, we can stop here
        return;
    }
    // Now iterate from the second pixel to the end, population distinct colors
    for (int i = 1; i < pixelCount; ++i) {
        SIGColor color = pixels[i];
        if (color == currentColor) {
            // We've hit the same color as before, increase population
            int hval = histo->mColorCounts[currentColorIndex];
            histo->mColorCounts[currentColorIndex] = hval + 1;
        }
        else {
            // We've hit a new color, increase index
            currentColor = color;
            currentColorIndex++;
            histo->mColors[currentColorIndex] = currentColor;
            histo->mColorCounts[currentColorIndex] = 1;
        }
    }
    
    SIGColorHash *hash = NULL;
    // populate the color indexes
    for (int i = 0; i < histo->mNumColors; ++i) {
        SIGColor color = histo->mColors[i];
        hash = SIGColorHistogramHashColor(hash, color, histo->mColorCounts[i]);
    }
    histo->mColorHash = hash;
}

void SIGColorHistogramDestroy(SIGColorHistogram *histo) {
    free(histo->mColors);
    free(histo->mColorCounts);
    free(histo);
}

int SIGColorHistogramGetColorIndex(int red, int green, int blue) {
    return 65536 * red + 256 * green + blue;
//    red = red >> RSHIFT;
//    green = green >> RSHIFT;
//    blue = blue >> RSHIFT;
//    return (red << (2 * SIGBITS)) + (green << SIGBITS) + blue;
}

int SIGColorHistogramGetColorIndexForColor(SIGColor color) {
    return SIGColorHistogramGetColorIndex(SIGColorGetRed32(color), SIGColorGetGreen32(color), SIGColorGetBlue32(color));
}

int SIGColorHistogramCountForColor(SIGColorHistogram histo, SIGColor color) {
    struct SIGColorHash *s = SIGColorHistogramGetHashColor(histo.mColorHash, color);
    return s->population;
}
