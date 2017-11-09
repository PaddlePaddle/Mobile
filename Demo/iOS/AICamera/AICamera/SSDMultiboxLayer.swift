//
//  SSDMultiboxLayer.swift
//  SSDDemo
//
//  Created by Wang,Jeff on 11/7/17.
//  Copyright © 2017 PaddlePaddle. All rights reserved.
//

import UIKit

class SSDMultiboxLayer: CALayer {
    var boxLayers = Array<SSDDrawLayer>()
    var allDatas = Array<SSDData>()
    
    func displayBoxs(with datas: Array<SSDData>){
        self.sublayers?.forEach({ (layer) in
            layer.removeFromSuperlayer()
        })
        
        for data: SSDData in datas {
            let boxLayer = SSDDrawLayer.init()
            boxLayer.render(data)
            
            self.addSublayer(boxLayer)
        }
    }
}
