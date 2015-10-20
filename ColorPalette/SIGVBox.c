//
//  SIGVBox.c
//  Signals
//
//  Created by Andrew Poes on 7/15/15.
//  Copyright (c) 2015 Signals. All rights reserved.
//

#include <stdlib.h>
#include "SIGVBox.h"

SIGVBox SIGVboxMake(SIGColor *colors, int totalColors, SIGColorHistogram histo) {
    SIGVBox vbox;
    SIGVboxInit(&vbox, colors, totalColors, histo);
    SIGVBoxFitBox(&vbox);
    return vbox;
}

void SIGVboxInit(SIGVBox *vbox, SIGColor *colors, int totalColors, SIGColorHistogram histo) {
    SIGColor *vboxColors = calloc(totalColors, sizeof(SIGColor));
    for (uint32_t i = 0; i < totalColors; ++i) {
        vboxColors[i] = colors[i];
    }
    vbox->mNumColors = totalColors;
    vbox->mColors = vboxColors;
    vbox->mHisto = histo;
}

void SIGVboxDestroy(SIGVBox *vbox) {
    free(vbox->mColors);
    free(vbox);
}

void SIGVBoxFitBox(SIGVBox *vbox) {
    uint32_t totalColors = SIGVBoxGetColorCount(*vbox);
    // Reset the min and max to opposite values
    uint32_t mMinRed = 255;
    uint32_t mMinGreen = 255;
    uint32_t mMinBlue = 255;
    uint32_t mMaxRed = 0;
    uint32_t mMaxGreen = 0;
    uint32_t mMaxBlue = 0;
    
    for (int i = 0; i < totalColors; ++i) {
        SIGColor color = vbox->mColors[i];
        uint32_t r = SIGColorGetRed32(color);
        uint32_t g = SIGColorGetGreen32(color);
        uint32_t b = SIGColorGetBlue32(color);
        
        if (r > mMaxRed) {
            mMaxRed = r;
        }
        if (r < mMinRed) {
            mMinRed = r;
        }
        if (g > mMaxGreen) {
            mMaxGreen = g;
        }
        if (g < mMinGreen) {
            mMinGreen = g;
        }
        if (b > mMaxBlue) {
            mMaxBlue = b;
        }
        if (b < mMinBlue) {
            mMinBlue = b;
        }
    }
    vbox->mMinRed = mMinRed;
    vbox->mMaxRed = mMaxRed;
    vbox->mMinGreen = mMinGreen;
    vbox->mMaxGreen = mMaxGreen;
    vbox->mMinBlue = mMinBlue;
    vbox->mMaxBlue = mMaxBlue;
}

uint32_t SIGVBoxGetVolume(SIGVBox vbox) {
    return (vbox.mMaxRed - vbox.mMinRed + 1) * (vbox.mMaxGreen - vbox.mMinGreen + 1) * (vbox.mMaxBlue - vbox.mMinBlue + 1);
}

uint32_t SIGVBoxCanSplit(SIGVBox vbox) {
    return vbox.mNumColors > 1;
}

uint32_t SIGVBoxGetColorCount(SIGVBox vbox) {
    return vbox.mNumColors;
}

/**
 * Split this color box at the mid-point along it's longest dimension
 *
 * @return the new ColorBox
 */
SIGVboxStatusCode SIGVBoxSplitBox(SIGVBox *vbox, SIGVBox *newVbox) {
    if (SIGVBoxCanSplit(*vbox)) {
        uint32_t splitPoint = SIGVBoxFindSplitPoint(vbox);
        uint32_t totalColors = vbox->mNumColors;
        uint32_t splitTotalColors = totalColors - splitPoint;
        if (splitTotalColors > 1) {
            SIGColor *newVboxColors = calloc(splitTotalColors, sizeof(SIGColor));
            uint32_t colorIndex = 0;
            for (uint32_t i = splitPoint; i < vbox->mNumColors; ++i) {
                newVboxColors[colorIndex++] = vbox->mColors[i];
            }
            newVbox->mNumColors = splitTotalColors;
            newVbox->mColors = newVboxColors;
            newVbox->mHisto = vbox->mHisto;
            SIGVBoxFitBox(newVbox);
            
            uint32_t mNumColors = totalColors - splitTotalColors;
            SIGColor *vboxColors = calloc(totalColors, sizeof(SIGColor));
            for (uint32_t i = 0; i < totalColors; ++i) {
                vboxColors[i] = vbox->mColors[i];
            }
            free(vbox->mColors);
            vbox->mColors = vboxColors;
            vbox->mNumColors = mNumColors;
            SIGVBoxFitBox(vbox);
            
            // yay we successfully split the vbox!
            return kSIGVboxStatusCodeInitializationSuccess;
        }
    }
    // Can not split a box with only 1 color
    return kSIGVboxStatusCodeInitializationFailure;
}

/**
 * @return the dimension which this box is largest in
 */
SIGColorComponent SIGVboxGetLongestColorDimension(SIGVBox vbox) {
    uint32_t redLength = vbox.mMaxRed - vbox.mMinRed;
    uint32_t greenLength = vbox.mMaxGreen - vbox.mMinGreen;
    uint32_t blueLength = vbox.mMaxBlue - vbox.mMinBlue;
    
    if (redLength >= greenLength && redLength >= blueLength) {
        return SIGColorComponentRed;
    } else if (greenLength >= redLength && greenLength >= blueLength) {
        return SIGColorComponentGreen;
    } else {
        return SIGColorComponentBlue;
    }
}

/**
 * @return the midpoint of this box in the given {@code dimension}
 */
uint32_t SIGVboxGetMidPointForComponent(SIGVBox vbox, SIGColorComponent component) {
    switch (component) {
        case SIGColorComponentGreen: return (vbox.mMinGreen + vbox.mMaxGreen) / 2;
        case SIGColorComponentBlue:  return (vbox.mMinBlue + vbox.mMaxBlue)   / 2;
        case SIGColorComponentRed:
        default:                     return (vbox.mMinRed + vbox.mMaxRed)     / 2;
    }
}

/**
 * Finds the point within this box's lowerIndex and upperIndex index of where to split.
 *
 * This is calculated by finding the longest color dimension, and then sorting the
 * sub-array based on that dimension value in each color. The colors are then iterated over
 * until a color is found with at least the midpoint of the whole box's dimension midpoint.
 *
 * @return the index of the colors array to split from
 */
uint32_t SIGVBoxFindSplitPoint(SIGVBox *vbox) {
    uint32_t splitPoint = 0;
    
    // We need to sort the colors in this box based on the longest color dimension.
    SIGColorComponent component = SIGVboxGetLongestColorDimension(*vbox);
    SIGColor *mColors = vbox->mColors;
    uint32_t totalColors = vbox->mNumColors;
    if (component == SIGColorComponentRed) {
        qsort(mColors, totalColors, sizeof(SIGColor), SIGColorCompareRed);
    } else if (component == SIGColorComponentGreen) {
        qsort(mColors, totalColors, sizeof(SIGColor), SIGColorCompareGreen);
    } else {
        qsort(mColors, totalColors, sizeof(SIGColor), SIGColorCompareBlue);
    }
    // set the sorted colors back
    vbox->mColors = mColors;
    uint32_t midPoint = SIGVboxGetMidPointForComponent(*vbox, component);
    for (uint32_t i = 0; i < vbox->mNumColors; ++i) {
        SIGColor color = vbox->mColors[i];
        if (component == SIGColorComponentRed) {
            if (SIGColorGetRed32(color) >= midPoint) {
                splitPoint = i;
                break;
            }
        } else if (component == SIGColorComponentGreen) {
            if (SIGColorGetGreen32(color) >= midPoint) {
                splitPoint = i;
                break;
            }
        } else if (component == SIGColorComponentBlue) {
            if (SIGColorGetBlue32(color) >= midPoint) {
                splitPoint = i;
                break;
            }
        }
    }
    
    return splitPoint;
}

/**
 * @return the average color of this box.
 */
SIGSwatch SIGVboxGetAverageColor(SIGVBox vbox) {
    uint32_t redSum = 0;
    uint32_t greenSum = 0;
    uint32_t blueSum = 0;
    uint32_t totalPopulation = 0;
    
    for (uint32_t i = 0; i < vbox.mNumColors; ++i) {
        SIGColor color = vbox.mColors[i];
        uint32_t colorPopulation = SIGColorHistogramCountForColor(vbox.mHisto, color);
        totalPopulation += colorPopulation;
        uint32_t red = SIGColorGetRed32(color);
        uint32_t green = SIGColorGetGreen32(color);
        uint32_t blue = SIGColorGetBlue32(color);
        redSum += colorPopulation * red;
        greenSum += colorPopulation * green;
        blueSum += colorPopulation * blue;
    }
    
    if (totalPopulation == 0) {
        return SIGSwatchMake(0, 0, 0, 0);
    }
    
    // println("\(redSum) \(greenSum) \(blueSum)")
    uint32_t redAverage = redSum / totalPopulation;
    uint32_t greenAverage = greenSum / totalPopulation;
    uint32_t blueAverage = blueSum / totalPopulation;
    return SIGSwatchMake(redAverage, greenAverage, blueAverage, totalPopulation);
}

int __IntCompare(int a, int b) {
    if ( a == b ) return 0;
    else if ( a < b ) return -1;
    else return 1;
}

int SIGVboxCompareVolume( const void* a, const void* b) {
    SIGVBox vbox_a = * ( (SIGVBox*) a );
    SIGVBox vbox_b = * ( (SIGVBox*) b );
    return __IntCompare(SIGVBoxGetVolume(vbox_a), SIGVBoxGetVolume(vbox_b));
}
