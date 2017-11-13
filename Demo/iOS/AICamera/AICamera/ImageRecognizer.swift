//
//  ImageRecognizer.swift
//  SSDDemo
//
//  Created by Nicky Chan on 11/7/17.
//  Copyright © 2017 PaddlePaddle. All rights reserved.
//

import Foundation

protocol ImageRecognizerDelegate {
    func imageRecognizedSuccess(_ result: SSDData)
    func imageRecognizedError()
}

class ImageRecognizer {
    
    var imageRecognizer: ImageRecognizerPaddleWrapper?
    
    init() {
        imageRecognizer = ImageRecognizerPaddleWrapper()
    }
    
    func inference(imageBuffer: UnsafeMutablePointer<UInt8>!, width: Int32, height: Int32) -> NSMutableArray! {
        
        return imageRecognizer?.inference(imageBuffer, withHeight: height, withWidth: width)
    }
    
    func release() {
        imageRecognizer?.destroy()
    }
    
}




