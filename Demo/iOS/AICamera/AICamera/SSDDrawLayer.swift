//
//  SSDDrawLayer.swift
//  SSDDemo
//
//  Created by Nicky Chan on 11/7/17.
//  Copyright © 2017 PaddlePaddle. All rights reserved.
//

import UIKit

class SSDDrawLayer: CAShapeLayer {
    var labelLayer = CATextLayer()
    
    required override init() {
        super.init()

        labelLayer.fontSize = 18
        labelLayer.contentsScale = UIScreen.main.scale
        labelLayer.foregroundColor = UIColor.blue.cgColor
        addSublayer(labelLayer)
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    func render(_ data: SSDData) {
        
        self.path = UIBezierPath(roundedRect: CGRect(x: data.x, y: data.y, width: data.width, height: data.height), cornerRadius: 10).cgPath
        self.strokeColor = UIColor.blue.cgColor
        self.lineWidth = 3.0
        self.fillColor = nil
        self.lineJoin = kCALineJoinBevel
    
        labelLayer.string = String.init(format: "%@: %@", data.label, String(data.accuracy))
        labelLayer.fontSize = 18
        labelLayer.frame = CGRect.init(x: data.x, y: data.y + data.height, width: 100, height: 20)
    }
}
