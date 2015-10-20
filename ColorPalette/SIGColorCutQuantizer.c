//
//  SIGColorCutQuantizer.c
//  Signals
//
//  Created by Andrew Poes on 7/9/15.
//  Copyright (c) 2015 Signals. All rights reserved.
//

#include <stdlib.h>
#include "SIGColorCutQuantizer.h"

const double kSIGColorCutBlackMaxLightness = 0.05;
const double kSIGColorCutWhiteMinLightness = 0.95;

/*
 * Color Cut Quantizer
 */
SIGColorCutQuantizer SIGColorCutQuantizerMake(SIGColor *colors, int numColors, int maxColors) {
    SIGColorCutQuantizer quantizer;
    /*
    // sort the colors lowest to highest
    qsort(colors, numColors, sizeof(SIGColor), SIGColorCompare);
    // create the histogram
    SIGColorHistogram histo = SIGColorHistogramMake(colors, numColors);
    quantizer.mHisto = histo;
    // Now go through all of the colors and keep those which we do not want to ignore
    // First create the color array
    SIGColor *mColors = malloc(sizeof(SIGColor) * numColors);
    int validColorCount = 0;
    SIGColor currentColor = 0;
    // Now iterate from to the end, adding distinct valid colors
    for (int i = 0; i < numColors; ++i) {
        if (colors[i] != currentColor) {
            currentColor = colors[i];
            bool shouldIgnore = SIGColorShouldIgnore(currentColor);
            if (shouldIgnore == false) {
                mColors[validColorCount++] = currentColor;
            }
        }
    }
    // realloc to appropriate memory footprint
    mColors = realloc(mColors, sizeof(SIGColor) * validColorCount);
    // set the colors / count on the quantizer struct
    quantizer.mColors = mColors;
    quantizer.mNumColors = validColorCount;
    //
     */
    SIGColorHistogram histo = SIGColorHistogramMakeFromColorArray(colors, numColors);
    quantizer.mHisto = histo;
    // Now go through all of the colors and keep those which we do not want to ignore
    // First create the color array
    int validColorCount = 0;
    for (int i = 0; i < numColors; ++i) {
        SIGColor color = colors[i];
        bool shouldIgnore = SIGColorShouldIgnore(color);
        if (shouldIgnore == false) {
            ++validColorCount;
        }
    }
    int colorIndex = 0;
    SIGColor *mColors = calloc(validColorCount, sizeof(SIGColor));
    for (int i = 0; i < numColors; ++i) {
        SIGColor color = colors[i];
        bool shouldIgnore = SIGColorShouldIgnore(color);
        if (shouldIgnore == false) {
            mColors[colorIndex++] = color;
        }
    }
    // set the colors / count on the quantizer struct
    quantizer.mColors = mColors;
    quantizer.mNumColors = validColorCount;
    
    
    if (validColorCount <= maxColors) {
        // The image has fewer colors than the maximum requested, so just return the colors
        SIGSwatch mQuantizedColors[validColorCount];
        for (int i = 0; i < validColorCount; ++i) {
            SIGColor color = mColors[i];
            int colorPopulation = SIGColorHistogramCountForColor(histo, color);
            uint32_t red = SIGColorGetRed32(color);
            uint32_t green = SIGColorGetGreen32(color);
            uint32_t blue = SIGColorGetBlue32(color);
            SIGSwatch swatch = SIGSwatchMake(red, green, blue, colorPopulation);
            mQuantizedColors[i] = swatch;
        }
        quantizer.mQuantizedColors = mQuantizedColors;
        quantizer.mTotalQuantizedColors = validColorCount;
    }
    else {
        // We need use quantization to reduce the number of colors
        int totalSwatches = 0;
        SIGSwatch *mQuantizedColors = SIGColorCutQuantizerQuantizePixels(quantizer, maxColors, &totalSwatches);
        quantizer.mQuantizedColors = mQuantizedColors;
        quantizer.mTotalQuantizedColors = totalSwatches;
    }
    
    return quantizer;
}

SIGSwatch* SIGColorCutQuantizerQuantizePixels(SIGColorCutQuantizer quantizer, int maxColors, int *totalSwatches) {
    // Create the priority queue which is sorted by volume descending. This means we always
    // split the largest box in the queue
    int boxLen = 1;
    SIGVBox *boxes = calloc(maxColors, sizeof(SIGVBox));
    // To start, offer a box which contains all of the colors
    SIGVBox vbox = SIGVboxMake(quantizer.mColors, quantizer.mNumColors, quantizer.mHisto);
    boxes[0] = vbox;
    // Now go through the boxes, splitting them until we have reached maxColors or there are no more boxes to split
    SIGColorCutQuantizerSplitBoxes(boxes, &boxLen, maxColors);
    // Finally, return the average colors of the color boxes
    SIGSwatch *swatches = SIGColorCutQuantizerGenerateAverageColors(boxes, boxLen, totalSwatches);
    // free the priority queue
    free(boxes);
    return swatches;
}

SIGSwatch* SIGColorCutQuantizerGenerateAverageColors(SIGVBox *boxes, int numBoxes, int *totalSwatches) {
    int numColors = 0;
    // Loop through and count valid colors
    for (int i = 0; i < numBoxes; ++i) {
        SIGVBox vbox = boxes[i];
        SIGSwatch avg = SIGVboxGetAverageColor(vbox);
        if (SIGSwatchShouldIgnore(avg) == false) {
            ++numColors;
        }
    }
    SIGSwatch *swatches;
    if (numColors > 0) {
        // allocate the array
        swatches = calloc(numColors, sizeof(SIGSwatch));
        // loop through and append colors to swatches
        numColors = 0;
        for (int i = 0; i < numBoxes; ++i) {
            SIGVBox vbox = boxes[i];
            SIGSwatch avg = SIGVboxGetAverageColor(vbox);
            if (SIGSwatchShouldIgnore(avg) == false) {
                swatches[numColors] = avg;
                ++numColors;
            }
        }
        // set total swatches
        *totalSwatches = numColors;
    }
    // return the swatches
    return swatches;
}

/**
 * Iterate through the {@link java.util.Queue}, popping
 * {@link ColorCutQuantizer.Vbox} objects from the queue
 * and splitting them. Once split, the new box and the remaining box are offered back to the
 * queue.
 *
 * @param queue {@link java.util.PriorityQueue} to poll for boxes
 * @param maxSize Maximum amount of boxes to split
 */
void SIGColorCutQuantizerSplitBoxes(SIGVBox *boxes, int *boxesLength, int maxColors) {
    while (*boxesLength < maxColors) {
        SIGVBox vbox = boxes[*boxesLength - 1];
        SIGVBox split;
        SIGVboxStatusCode statusCode = SIGVBoxSplitBox(&vbox, &split);
        if (statusCode == kSIGVboxStatusCodeInitializationFailure) {
            return;
        } else {
            *boxesLength = *boxesLength + 1;
            boxes[*boxesLength - 2] = vbox; // set the second to last item to be the existing vbox
            boxes[*boxesLength - 1] = split; // set the last item to be the new vbox
            qsort(boxes, *boxesLength, sizeof(SIGVBox), SIGVboxCompareVolume);
        }
    }
}

/*
 * SIGColor Color Ignore funcs
 */

bool SIGColorShouldIgnore(SIGColor color) {
    uint32_t red = SIGColorGetRed32(color);
    uint32_t green = SIGColorGetGreen32(color);
    uint32_t blue = SIGColorGetBlue32(color);
    SIGSwatch swatch = SIGSwatchMake(red, green, blue, 0);
    bool shouldIgnore = SIGSwatchShouldIgnore(swatch);
    return shouldIgnore;
}

bool SIGSwatchShouldIgnore(SIGSwatch swatch) {
    SIGHSL hsl = SIGSwatchGetHSL(swatch);
    bool isBlack = SIGHSLIsBlack(hsl, kSIGColorCutBlackMaxLightness);
    bool isWhite = SIGHSLIsWhite(hsl, kSIGColorCutWhiteMinLightness);
    bool isNearRedILine = SIGHSLIsNearRedILine(hsl);
    return  isWhite || isBlack || isNearRedILine;
}
