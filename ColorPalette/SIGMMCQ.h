//
//  SIGMMCQ.h
//  Signals
//
//  Created by Andrew Poes on 6/7/15.
//  Copyright (c) 2015 Signals. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SIGColor.h"

@interface SIGMMCQ : NSObject

//- (NSArray *)colorPaletteForImage:(UIImage *)image imageScale:(CGFloat)scale paletteCount:(NSUInteger)paletteCount pixelSkip:(NSUInteger)pixelSkip;
- (NSArray *)colorPaletteForColorArray:(SIGColor *)pixels pixelCount:(int)pixelCount;

@end
