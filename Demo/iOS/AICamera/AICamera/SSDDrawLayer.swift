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
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    func render(_ data: SSDData) {
        
        let screenWidth = UIScreen.main.bounds.size.width
        let screenHeight = UIScreen.main.bounds.size.height
        
//        //have to swap height and width because the x, y is swapped
        let x = CGFloat(data.xmin) * screenWidth
        let y = CGFloat(data.ymin) * screenHeight // 1 - ymax because its y is opposite
        let width = CGFloat(data.xmax - data.xmin) * screenWidth
        let height = CGFloat(data.ymax - data.ymin) * screenHeight
        
        
        self.path = UIBezierPath(roundedRect: CGRect(x: x, y: y, width: width, height: height), cornerRadius: 10).cgPath
        self.strokeColor = UIColor.cyan.cgColor
        self.lineWidth = 3.0
        self.fillColor = nil
        self.lineJoin = kCALineJoinBevel
    
        labelLayer.string = String.init(format: "%@: %.02f", data.label, data.accuracy)
        labelLayer.fontSize = 17
        labelLayer.contentsScale = UIScreen.main.scale
        labelLayer.foregroundColor = UIColor.cyan.cgColor
        labelLayer.frame = CGRect.init(x: x + 4, y: y + height - 22, width: 1000, height: 30)
        addSublayer(labelLayer)
    }
}
