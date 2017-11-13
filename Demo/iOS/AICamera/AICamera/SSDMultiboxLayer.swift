//
//  SSDMultiboxLayer.swift
//  SSDDemo
//
//  Created by Wang,Jeff on 11/7/17.
//  Copyright © 2017 PaddlePaddle. All rights reserved.
//

import UIKit

class SSDMultiboxLayer: CALayer {
    
    func displayBoxs(with ssdDataList: NSMutableArray){
        self.sublayers?.forEach({ (layer) in
            layer.removeFromSuperlayer()
        })
        
        for ssdData in ssdDataList {
            let boxLayer = SSDDrawLayer.init()
            boxLayer.render(ssdData as! SSDData, rectSize: self.visibleRect)
            
            self.addSublayer(boxLayer)
        }
    }
}
