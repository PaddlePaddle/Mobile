//
//  SSDData.swift
//  SSDDemo
//
//  Created by Nicky Chan on 11/7/17.
//  Copyright © 2017 PaddlePaddle. All rights reserved.
//

import Foundation
import UIKit

class SSDData {
    
    var x : CGFloat = 0
    var y : CGFloat = 0
    var width : CGFloat = 0
    var height : CGFloat = 0
    var label : String = ""
    var accuracy : Float = 0
    
    var xmin : Float = 0
    var ymin : Float = 0
    var xmax : Float = 0
    var ymax : Float = 0
    
    init(with label: String, accuracy: Float, xmin: Float, ymin: Float, xmax: Float, ymax: Float, rectSize: CGSize) {
        self.label = label
        self.accuracy = accuracy
        self.xmin = xmin
        self.ymin = ymin
        self.xmax = xmax
        self.ymax = ymax
        
        x = CGFloat(self.xmin) * rectSize.width
        y = CGFloat(self.ymin) * rectSize.height
        width = CGFloat(self.xmax - self.xmin) * rectSize.width
        height = CGFloat(self.ymax - self.ymin) * rectSize.height
    }
}
