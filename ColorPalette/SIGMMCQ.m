//
//  SIGMMCQ.m
//  Signals
//
//  Created by Andrew Poes on 6/7/15.
//  Copyright (c) 2015 Signals. All rights reserved.
//

#import "SIGMMCQ.h"

static const int SIGBITS = 5;
static const int RSHIFT = 8 - SIGBITS;
static const int MULT = 1 << RSHIFT;
static const int HISTOSIZE = 1 << (3 * SIGBITS);
static const int VBOX_LENGTH = 1 << SIGBITS;
static const double FRACT_BY_POPULATION = 0.75;
static const int MAX_ITERATIONS = 1000;

static int getColorIndex(int r, int g, int b)
{
    return (r << (2 * SIGBITS)) + (g << SIGBITS) + b;
}

#pragma mark - VBox

@interface VBox : NSObject
{
    int r1;
    int r2;
    int g1;
    int g2;
    int b1;
    int b2;
    int histo[HISTOSIZE];
    int avg[3];
    BOOL _avgSet;
    int _volume;
    int _count;
}

- (instancetype)initWithR1:(int)_r1 r2:(int)_r2 g1:(int)_g1 g2:(int)_g2 b1:(int)_b1 b2:(int)_b2 histo:(int *)_histo;
- (int)volume:(BOOL)force;
- (int)count:(BOOL)force;
- (int *)avg:(BOOL)force;
- (int)getR1;
- (void)setR1:(int)a;
- (int)getR2;
- (void)setR2:(int)a;
- (int)getG1;
- (void)setG1:(int)a;
- (int)getG2;
- (void)setG2:(int)a;
- (int)getB1;
- (void)setB1:(int)a;
- (int)getB2;
- (void)setB2:(int)a;

@end

#pragma mark - CMap

@interface CMap : NSObject
{
    NSMutableArray *vboxes;
}

- (int)size;
- (void)push:(VBox *)box;
- (NSArray *)palette;
- (int *)map:(SIGColor)color;
- (int *)nearest:(SIGColor)color;

@end

#pragma mark - VBox

@implementation VBox

- (instancetype)initWithR1:(int)_r1 r2:(int)_r2 g1:(int)_g1 g2:(int)_g2 b1:(int)_b1 b2:(int)_b2 histo:(int *)_histo
{
    self = [super init];
    if (self) {
        r1 = _r1;
        r2 = _r2;
        g1 = _g1;
        g2 = _g2;
        b1 = _b1;
        b2 = _b2;
        
        for (int i = 0; i < HISTOSIZE; i++) {
            histo[i] = _histo[i];
        }
    }
    return self;
}

- (int)getR1
{
    return r1;
}
- (void)setR1:(int)a
{
    r1 = a;
}

- (int)getR2
{
    return r2;
}
- (void)setR2:(int)a
{
    r2 = a;
}

- (int)getG1
{
    return g1;
}
- (void)setG1:(int)a
{
    g1 = a;
}

- (int)getG2
{
    return g2;
}
- (void)setG2:(int)a
{
    g2 = a;
}

- (int)getB1
{
    return b1;
}
- (void)setB1:(int)a
{
    b1 = a;
}

- (int)getB2
{
    return b2;
}
- (void)setB2:(int)a
{
    b2 = a;
}

- (int)volume:(BOOL)force
{
    if (_volume == 0 || force) {
        _volume = ((r2 - r1 + 1) * (g2 - g1 + 1) * (b2 - b1 + 1));
    }
    return _volume;
}

- (int)count:(BOOL)force
{
    if (_count == 0 || force) {
        int npix = 0;
        int i, j, k, index;
        
        for (i = r1; i <= r2; i++) {
            for (j = g1; j <= g2; j++) {
                for (k = b1; k <= b2; k++) {
                    index = getColorIndex(i, j, k);
                    npix += histo[index];
                }
            }
        }
        
        _count = npix;
    }
    return _count;
}

- (id)copy
{
    VBox *box = [VBox new];
    box->r1 = r1;
    box->r2 = r2;
    box->g1 = g1;
    box->g2 = g2;
    box->b1 = b1;
    box->b2 = b2;
    
    // copy histogram
    for (int i = 0; i < HISTOSIZE; i++) {
        box->histo[i] = histo[i];
    }
    
    return box;
}

- (int *)avg:(BOOL)force
{
    if (!_avgSet || force) {
        _avgSet = YES;
        
        int ntot = 0;
        
        int rsum = 0;
        int gsum = 0;
        int bsum = 0;
        
        int hval, i, j, k, histoindex;
        
        for (i = r1; i <= r2; i++)
        {
            for (j = g1; j <= g2; j++)
            {
                for (k = b1; k <= b2; k++)
                {
                    histoindex = getColorIndex(i, j, k);
                    hval = histo[histoindex];
                    ntot += hval;
                    rsum += (hval * (i + 0.5) * MULT);
                    gsum += (hval * (j + 0.5) * MULT);
                    bsum += (hval * (k + 0.5) * MULT);
                }
            }
        }
        
        if (ntot > 0)
        {
            avg[0] = ~~(rsum / ntot);
            avg[1] = ~~(gsum / ntot);
            avg[2] = ~~(bsum / ntot);
        }
        else
        {
            avg[0] = ~~(MULT * (r1 + r2 + 1) / 2);
            avg[1] = ~~(MULT * (g1 + g2 + 1) / 2);
            avg[2] = ~~(MULT * (b1 + b2 + 1) / 2);
        }
    }
    
    return avg;
}

- (BOOL)contains:(SIGColor)color
{
    int rval = SIGColorGetRed32(color) >> RSHIFT;
    int gval = SIGColorGetGreen32(color) >> RSHIFT;
    int bval = SIGColorGetBlue32(color) >> RSHIFT;
    
    return (rval >= r1 && rval <= r2 && gval >= g1 && gval <= g2
            && bval >= b1 && bval <= b2);
}

//- (NSComparisonResult)compare:(VBox *)other{
//    if ([self count:NO] * [self volume:NO] > [other count:NO] * [other volume:NO]) {
//        return NSOrderedDescending;
//    }
//    else if ([self count:NO] * [self volume:NO] < [other count:NO] * [other volume:NO]) {
//        return NSOrderedAscending;
//    }
//    return NSOrderedSame;
//}

- (NSComparisonResult)compareCount:(VBox *)other
{
    return [self count:NO] - [other count:NO];
}

- (NSComparisonResult)compareProduct:(VBox *)other
{
    int aCount = [self count:NO];
    int bCount = [other count:NO];
    int aVolume = [self volume:NO];
    int bVolume = [other volume:NO];
    
    if (aCount == bCount) {
        return aVolume - bVolume;
    }
    
    return aCount * aVolume - bCount * bVolume;
}

@end

#pragma mark - CMap

@implementation CMap

- (int)size
{
    return (int)vboxes.count;
}

- (void)push:(VBox *)box
{
    if (!vboxes) {
        vboxes = [NSMutableArray array];
    }
    [vboxes addObject:box];
}

- (NSArray *)palette
{
    NSMutableArray *palette = [NSMutableArray array];
    int numVBoxes = self.size;
    for (int i = 0; i < numVBoxes; i++) {
        int *avg = [[vboxes objectAtIndex:i] avg:NO];
        NSArray *colorArray = @[ @(avg[0]), @(avg[1]), @(avg[2]) ];
        [palette addObject:colorArray];
    }
    return palette;
}

- (int *)map:(SIGColor)color
{
    int numVBoxes = self.size;
    for (int i = 0; i < numVBoxes; i++) {
        VBox *vbox = [vboxes objectAtIndex:i];
        if ([vbox contains:color]) {
            return [vbox avg:NO];
        }
    }
    return [self nearest:color];
}

- (int *)nearest:(SIGColor)color
{
    double d1 = DBL_MAX;
    double d2;
    int *pColor = NULL;
    
    int numVBoxes = self.size;
    for (int i = 0; i < numVBoxes; i++) {
        int *vbColor = [[vboxes objectAtIndex:i] avg:NO];
        d2 = sqrt(  pow(SIGColorGetRed32(color) - vbColor[0], 2)
                  + pow(SIGColorGetGreen32(color) - vbColor[1], 2)
                  + pow(SIGColorGetBlue32(color) - vbColor[2], 2));
        if (d2 < d1) {
            d1 = d2;
            pColor = vbColor;
        }
    }
    return pColor;
}

#pragma mark -

int* getHisto(SIGColor *pixels, int numPixels)
{
    int *histo = calloc(HISTOSIZE, sizeof(int));
    int index, rval, gval, bval;
    
    for (int i = 0; i < numPixels; i++) {
        SIGColor pixel = pixels[i];
        rval = SIGColorGetRed32(pixel) >> RSHIFT;
        gval = SIGColorGetGreen32(pixel) >> RSHIFT;
        bval = SIGColorGetBlue32(pixel) >> RSHIFT;
        index = getColorIndex(rval, gval, bval);
        histo[index]++;
    }
    return histo;
}

#pragma mark -

VBox* vboxFromPixels(SIGColor *pixels, int numPixels, int *histo)
{
    int rmin = 1000000, rmax = 0;
    int gmin = 1000000, gmax = 0;
    int bmin = 1000000, bmax = 0;
    
    int rval, gval, bval;
    
    // find min/max
    for (int i = 0; i < numPixels; i++) {
        SIGColor pixel = pixels[i];
        rval = SIGColorGetRed32(pixel) >> RSHIFT;
        gval = SIGColorGetGreen32(pixel) >> RSHIFT;
        bval = SIGColorGetBlue32(pixel) >> RSHIFT;
        
        if (rval < rmin) {
            rmin = rval;
        }
        else if (rval > rmax) {
            rmax = rval;
        }
        
        if (gval < gmin) {
            gmin = gval;
        }
        else if (gval > gmax) {
            gmax = gval;
        }
        
        if (bval < bmin) {
            bmin = bval;
        }
        else if (bval > bmax) {
            bmax = bval;
        }
    }
    
    return [[VBox alloc] initWithR1:rmin r2:rmax g1:gmin g2:gmax b1:bmin b2:bmax histo:histo];
}

NSArray* medianCutApply(int *histo, VBox *vbox)
{
    if ([vbox count:NO] == 0) {
        return nil;
    }
    
    // only one pixel, no split
    if ([vbox count:NO] == 1)
    {
        return @[ [vbox copy] ];
    }
    
    int rw = [vbox getR2] - [vbox getR1] + 1;
    int gw = [vbox getG2] - [vbox getG1] + 1;
    int bw = [vbox getB2] - [vbox getB1] + 1;
    int maxw = MAX(MAX(rw, gw), bw);
    
    // Find the partial sum arrays along the selected axis.
    int total = 0;
    int partialsum[VBOX_LENGTH];
    for (int i = 0; i < VBOX_LENGTH; ++i) {
        partialsum[i] = -1;
    }
    int lookaheadsum[VBOX_LENGTH];
    for (int i = 0; i < VBOX_LENGTH; ++i) {
        lookaheadsum[i] = -1;
    }
    int i, j, k, sum, index;
    
    if (maxw == rw) {
        for (i = [vbox getR1]; i <= [vbox getR2]; i++) {
            sum = 0;
            for (j = [vbox getG1]; j <= [vbox getG2]; j++) {
                for (k = [vbox getB1]; k <= [vbox getB2]; k++) {
                    index = getColorIndex(i, j, k);
                    sum += histo[index];
                }
            }
            total += sum;
            partialsum[i] = total;
        }
    }
    else if (maxw == gw) {
        for (i = [vbox getG1]; i <= [vbox getG2]; i++) {
            sum = 0;
            for (j = [vbox getR1]; j <= [vbox getR2]; j++) {
                for (k = [vbox getB1]; k <= [vbox getB2]; k++) {
                    index = getColorIndex(j, i, k);
                    sum += histo[index];
                }
            }
            total += sum;
            partialsum[i] = total;
        }
    }
    /* maxw == bw */
    else {
        for (i = [vbox getB1]; i <= [vbox getB2]; i++) {
            sum = 0;
            for (j = [vbox getR1]; j <= [vbox getR2]; j++) {
                for (k = [vbox getG1]; k <= [vbox getG2]; k++) {
                    index = getColorIndex(j, k, i);
                    sum += histo[index];
                }
            }
            total += sum;
            partialsum[i] = total;
        }
    }
    
    for (i = 0; i < VBOX_LENGTH; i++) {
        if (partialsum[i] != -1) {
            lookaheadsum[i] = total - partialsum[i];
        }
    }
    
    // determine the cut planes
    return maxw == rw ? doCut('r', vbox, partialsum, lookaheadsum, total)
    : maxw == gw ? doCut('g', vbox, partialsum, lookaheadsum, total)
    : doCut('b', vbox, partialsum, lookaheadsum, total);
}

NSArray* doCut(char color, VBox *vbox, int* partialsum, int* lookaheadsum, int total)
{
    int vbox_dim1;
    int vbox_dim2;
    
    if (color == 'r')
    {
        vbox_dim1 = [vbox getR1];
        vbox_dim2 = [vbox getR2];
    }
    else if (color == 'g')
    {
        vbox_dim1 = [vbox getG1];
        vbox_dim2 = [vbox getG2];
    }
    else
    /* color == 'b' */
    {
        vbox_dim1 = [vbox getB1];
        vbox_dim2 = [vbox getB2];
    }
    
    int left, right;
    VBox *vbox1 = nil, *vbox2 = nil;
    int d2, count2;
    
    for (int i = vbox_dim1; i <= vbox_dim2; i++) {
        if (partialsum[i] > total / 2) {
            vbox1 = [vbox copy];
            vbox2 = [vbox copy];
            
            left = i - vbox_dim1;
            right = vbox_dim2 - i;
            
            if (left <= right) {
                d2 = MIN(vbox_dim2 - 1, ~~(i + right / 2));
            }
            else {
                // 2.0 and cast to int is necessary to have the same
                // behaviour as in JavaScript
                d2 = MAX(vbox_dim1, ~~((int) (i - 1 - left / 2.0)));
            }
            
            // avoid 0-count boxes
            while (d2 < 0 || partialsum[d2] <= 0) {
                d2++;
            }
            count2 = lookaheadsum[d2];
            while (count2 == 0 && d2 > 0 && partialsum[d2 - 1] > 0) {
                count2 = lookaheadsum[--d2];
            }
            
            // set dimensions
            if (color == 'r') {
                [vbox1 setR2:d2];
                [vbox2 setR1:d2 + 1];
            }
            else if (color == 'g') {
                [vbox1 setG2:d2];
                [vbox2 setG1:d2 + 1];
            }
            /* color == 'b' */
            else {
                [vbox1 setB2:d2];
                [vbox2 setB1:d2 + 1];
            }
            
            return @[ vbox1, vbox2 ];
        }
    }
    
    NSLog(@"VBox can't be cut");
    return nil;
}

CMap* quantize(SIGColor *pixels, int numPixels, int maxcolors)
{
    // short-circuit
    if (numPixels == 0 || maxcolors < 2 || maxcolors > 256) {
        return nil;
    }
    
    int *histo = getHisto(pixels, numPixels);
    
    // get the beginning vbox from the colors
    VBox *vbox = vboxFromPixels(pixels, numPixels, histo);
    NSMutableArray *pq = [NSMutableArray array];
    [pq addObject:vbox];
    
    // Round up to have the same behaviour as in JavaScript
    int target = (int)ceil(FRACT_BY_POPULATION * maxcolors);
    
    // first set of colors, sorted by population
    iter(pq, @selector(compareCount:), target, histo);
    
    // Re-sort by the product of pixel occupancy times the size in color
    // space.
    [pq sortUsingSelector:@selector(compareProduct:)];
    
    // next set - generate the median cuts using the (npix * vol) sorting.
    iter(pq, @selector(compareProduct:), maxcolors - (int)pq.count, histo);
    
    // calculate the actual colors
    CMap *cmap = [CMap new];
    for (VBox *vb in [pq reverseObjectEnumerator].allObjects) {
        [cmap push:vb];
    }
    
    free(histo);
    
    return cmap;
}

void iter(NSMutableArray *lh, SEL compareSelector, int target, int *histo)
{
    int ncolors = 1;
    int niters = 0;
    VBox *vbox = nil;
    
    while (niters < MAX_ITERATIONS) {
        vbox = [lh lastObject];
        if ([vbox count:NO] == 0) {
            [lh sortUsingSelector:compareSelector];
            niters++;
            continue;
        }
        [lh removeLastObject];
        
        // do the cut
        NSArray *vboxes = medianCutApply(histo, vbox);
        VBox *vbox1 = vboxes.count >= 1 ? vboxes[0] : nil;
        VBox *vbox2 = vboxes.count >= 2 ? vboxes[1] : nil;
        
        if (vbox1 != nil) {
            [lh addObject:vbox1];
        }
        if (vbox2 != nil) {
            [lh addObject:vbox2];
            ncolors++;
        }
        [lh sortUsingSelector:compareSelector];
        
        if (ncolors >= target) {
            return;
        }
        if (niters++ > MAX_ITERATIONS) {
            return;
        }
    }
}

@end

#pragma mark - MMCQ

@implementation SIGMMCQ

//- (NSArray *)colorPaletteForImage:(UIImage *)image imageScale:(CGFloat)scale paletteCount:(NSUInteger)paletteCount pixelSkip:(NSUInteger)pixelSkip
//{
//    // convert the source to RGB8888 to ensure we can interpolate it
//    CGImageRef source = image.CGImage;
//    size_t imageWidth = CGImageGetWidth(source);
//    size_t imageHeight = CGImageGetHeight(source);
//    // scale the image size based on the quality
//    imageWidth *= scale;
//    imageHeight *= scale;
//    // create a colorspace in RGB
//    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
//    // create a holder for the bitmap data
//    size_t dataLength = imageWidth * imageHeight * 4;
//    unsigned char *bitmapData = (unsigned char*)calloc(dataLength, sizeof(unsigned char));
//    // base constants for the image data
//    size_t bytesPerPixel = 4;
//    size_t bytesPerRow = bytesPerPixel * imageWidth;
//    size_t bitsPerComponent = 8;
//    // create teh image context
//    CGContextRef context = CGBitmapContextCreate(bitmapData, imageWidth, imageHeight, bitsPerComponent, bytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
//    // set the interpolation quality for reduction
//    CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
//    // draw the image
//    CGContextDrawImage(context, CGRectMake(0, 0, imageWidth, imageHeight), source);
//    // get the converted image
//    //    CGImageRef converted = CGBitmapContextCreateImage(context);
//    // release the context and colorspace
//    CGContextRelease(context);
//    CGColorSpaceRelease(colorSpace);
//    
//    size_t pixelCount = dataLength / 4;
//    size_t pixelArrayTotal = pixelCount * (1.0 / pixelSkip);
//    size_t pixelIndex = 0;
//    
//    Color *pixels = calloc(pixelArrayTotal, sizeof(Color));
//    
//    
//    for (size_t index = 0; index < pixelCount; index += pixelSkip) {
//        size_t offset = index * 4;
//        unsigned char r,g,b,a;
//        r = bitmapData[offset + 0];
//        g = bitmapData[offset + 1];
//        b = bitmapData[offset + 2];
//        a = bitmapData[offset + 3];
//        
//        // find pixels who are mostly opaque
//        if (a > 124) {
//            // set the pixel data
//            Color c;
//            c.r = r;
//            c.g = g;
//            c.b = b;
//            // set it on the arry
//            pixels[pixelIndex] = c;
//            // incremenet the pixel index
//            ++pixelIndex;
//        }
//    }
//    
//    NSArray *palette = [self colorPaletteForColorArray:pixels pixelCount:(int)pixelIndex];
//    
//    free(pixels);
//    free(bitmapData);
//    
//    return palette;
//}

- (NSArray *)colorPaletteForColorArray:(SIGColor *)pixels pixelCount:(int)pixelCount
{
    // pixel index is length
    // pixels is arr
    CMap *cmap = quantize(pixels, pixelCount , 10);
    NSArray *colors = [cmap palette];
    NSMutableArray *paletteColors = [NSMutableArray new];
    for (NSArray *color in colors) {
        CGFloat r,g,b;
        r = [color[0] integerValue] / 255.0;
        g = [color[1] integerValue] / 255.0;
        b = [color[2] integerValue] / 255.0;
        UIColor *c = [UIColor colorWithRed:r green:g blue:b alpha:1];
        [paletteColors addObject:c];
    }
    //
    return paletteColors;
}

@end
