//
//  ViewController.swift
//  ColorPalette
//
//  Created by Andrew Poes on 10/19/15.
//  Copyright © 2015 Andrew Poes. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    var image: UIImage?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        loadImage()
        if let image = self.image {
            doColorAnalysis(image)
            doColorAnalysisV2(image)
        }
    }

    func loadImage() {
        if let path = NSBundle.mainBundle().pathForResource("photo1", ofType: "jpg") {
            if let image = UIImage(contentsOfFile: path) {
                let imageView = UIImageView(image: image)
                imageView.contentMode = UIViewContentMode.ScaleAspectFit
                imageView.frame = UIScreen.mainScreen().bounds
                self.view.addSubview(imageView)
                self.image = image
            }
        }
    }
    
    func doColorAnalysis(image: UIImage) {
        let start = CACurrentMediaTime()
        var running = start
        let colors = image.getPixelData()
        print("getPixelData, \(CACurrentMediaTime() - running)")
        running = CACurrentMediaTime()
        let len: Int32 = Int32(colors.count)
        let cPtr = UnsafeMutablePointer<SIGColor>(colors)
        running = CACurrentMediaTime()
        let quantizer = SIGColorCutQuantizerMake(cPtr, len, 11);
        print("quantize, \(CACurrentMediaTime() - running)")
        running = CACurrentMediaTime()
        print("total, \(CACurrentMediaTime() - start)")
        let vw = CGRectGetWidth(UIScreen.mainScreen().bounds) / CGFloat(quantizer.mTotalQuantizedColors);
        var x: CGFloat = 0;
        for colorIndex in 0 ..< quantizer.mTotalQuantizedColors {
            let swatch = quantizer.mQuantizedColors[Int(colorIndex)]
            let view = UIView(frame: CGRect(x: x, y: CGRectGetHeight(UIScreen.mainScreen().bounds) - 90, width: vw, height: 90))
            x += vw
            self.view.addSubview(view)
            view.backgroundColor = UIColor(red: CGFloat(swatch.red)/255, green: CGFloat(swatch.green)/255, blue: CGFloat(swatch.blue)/255, alpha: 1)
        }
    }
    
    func doColorAnalysisV2(image: UIImage) {
        let mmcq = SIGMMCQ()
        let colors = image.getPixelData()
        let len: Int32 = Int32(colors.count)
        let cPtr = UnsafeMutablePointer<SIGColor>(colors)
        let palette = mmcq.colorPaletteForColorArray(cPtr, pixelCount: len)
        let vw = CGRectGetWidth(UIScreen.mainScreen().bounds) / CGFloat(palette.count);
        var x: CGFloat = 0;
        for colorIndex in 0 ..< palette.count {
            let color = palette[colorIndex] as! UIColor;
            let view = UIView(frame: CGRect(x: x, y: CGRectGetHeight(UIScreen.mainScreen().bounds) - 184, width: vw, height: 90))
            x += vw
            self.view.addSubview(view)
            view.backgroundColor = color
        }
    }
}

extension UIImage {
    func getPixelData() -> [SIGColor] {
        var result = [SIGColor]()
        
        let bytesPerPixel = 4
        
        let pixelData = CGDataProviderCopyData(CGImageGetDataProvider(CGImage))
        let rawData = CFDataGetBytePtr(pixelData)
        let len = CFDataGetLength(pixelData)
        
        // Now your rawData contains the image data in the RGBA8888 pixel format.
        var byteIndex = 0
        for _ in 0..<len {
            if byteIndex >= len {
                break;
            }
            let red   = rawData[byteIndex]
            let green = rawData[byteIndex + 1]
            let blue  = rawData[byteIndex + 2]
            let alpha = rawData[byteIndex + 3]
            byteIndex += bytesPerPixel * 20
            
            let c = SIGColorFromRGBA8(red, green, blue, alpha)
//            if i < 10 {
//                let s = String(format: "%#08x: %#02x, %#02x, %#02x, %#02x", c, red, green, blue, alpha)
//                print(s)
//            }
            result.append(c)
        }
        
        return result
    }
}

