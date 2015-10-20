//
//  ColorPaletteTests.swift
//  ColorPaletteTests
//
//  Created by Andrew Poes on 10/19/15.
//  Copyright © 2015 Andrew Poes. All rights reserved.
//

import XCTest

class ColorPaletteTests: XCTestCase {
    lazy var viewController: ViewController = {
       return ViewController()
    }()
    
    override func setUp() {
        super.setUp()
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }
    
    func testExample() {
        // This is an example of a functional test case.
        // Use XCTAssert and related functions to verify your tests produce the correct results.
    }
    
    func testPerformanceExample() {
        // This is an example of a performance test case.
        self.viewController.loadImage()
        if let image = self.viewController.image {
            self.measureBlock {
                self.viewController.doColorAnalysis(image);
//                self.viewController.doColorAnalysisV2(image);
            }
            XCTAssert(true, "Success")
        }
    }
    
}
