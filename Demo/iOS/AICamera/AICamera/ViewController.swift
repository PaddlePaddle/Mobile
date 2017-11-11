//
//  ViewController.swift
//  SSDDemo
//
//  Created by Nicky Chan on 11/6/17.
//  Copyright © 2017 PaddlePaddle. All rights reserved.
//

import UIKit
import AVFoundation
import Foundation


class ViewController: UIViewController, AVCaptureVideoDataOutputSampleBufferDelegate {
    
    let captureSession = AVCaptureSession()
    var previewLayer : AVCaptureVideoPreviewLayer?
    var videoConnection: AVCaptureConnection!
    var captureDevice : AVCaptureDevice?
    
    let imageRecognizer = ImageRecognizer()
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        
        captureSession.sessionPreset = AVCaptureSessionPresetHigh //use high will cause memory issue
        
        captureDevice = AVCaptureDeviceDiscoverySession(deviceTypes: [AVCaptureDeviceType.builtInWideAngleCamera], mediaType: AVMediaTypeVideo, position: AVCaptureDevicePosition.back).devices.first
        
        // setup video device input
        do {
            let videoDeviceInput: AVCaptureDeviceInput
            do {
                videoDeviceInput = try AVCaptureDeviceInput(device: captureDevice)
            }
            catch {
                fatalError("Could not create AVCaptureDeviceInput instance with error: \(error).")
            }
            
            captureSession.beginConfiguration()
            guard captureSession.canAddInput(videoDeviceInput) else {
                fatalError("CaptureSession can not add input")
            }
            captureSession.addInput(videoDeviceInput)
        }
        
        // setup preview
        let previewContainer = self.view.layer
        let previewLayer = AVCaptureVideoPreviewLayer(session: captureSession)!
        previewLayer.frame = previewContainer.bounds
        previewLayer.contentsGravity = kCAGravityResizeAspectFill
        previewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill
        previewContainer.insertSublayer(previewLayer, at: 0)
        self.previewLayer = previewLayer
        
        let multiboxLayer = SSDMultiboxLayer()
        multiboxLayer.displayBoxs(with: self.fakeData())
        previewContainer.insertSublayer(multiboxLayer, at: 1)
        
        // setup video output
        do {
            let videoDataOutput = AVCaptureVideoDataOutput()
            videoDataOutput.videoSettings = [kCVPixelBufferPixelFormatTypeKey as AnyHashable as! String: NSNumber(value: kCVPixelFormatType_32BGRA)]
            videoDataOutput.alwaysDiscardsLateVideoFrames = true
            guard captureSession.canAddOutput(videoDataOutput) else {
                fatalError("CaptureSession can not add output")
            }
            captureSession.addOutput(videoDataOutput)
            
            captureSession.commitConfiguration()
            
            let queue = DispatchQueue(label: "com.paddlepaddle.SSDDemo")
            videoDataOutput.setSampleBufferDelegate(self, queue: queue)
            
            videoDataOutput.connection(withMediaType: AVMediaTypeVideo)
            
            
            captureSession.startRunning()
        }
    }
    
    func captureOutput(_ output: AVCaptureOutput, didDrop sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {
        print("didDrop")
    }
    
    func captureOutput(_ output: AVCaptureOutput, didOutputSampleBuffer sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {
        
        NSLog("didOutput")
        
        if let imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer) {
            CVPixelBufferLockBaseAddress(imageBuffer, CVPixelBufferLockFlags(rawValue: 0))
        
            let bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer)
            let width = CVPixelBufferGetWidth(imageBuffer)
            let height = CVPixelBufferGetHeight(imageBuffer)
            let baseAddress = CVPixelBufferGetBaseAddress(imageBuffer)
            
            let intBuffer = unsafeBitCast(baseAddress, to: UnsafeMutablePointer<UInt8>.self)
            
            let bufferSize = bytesPerRow * height
            print("bufferSize = \(bufferSize)")
            
            CVPixelBufferUnlockBaseAddress(imageBuffer, CVPixelBufferLockFlags(rawValue: 0))
            
            let x = 100
            let y = 100;
//
//            
            let b = intBuffer[(x*4)+(y*bytesPerRow)];
            let g = intBuffer[((x*4)+(y*bytesPerRow))+1];
            let r = intBuffer[((x*4)+(y*bytesPerRow))+2];
            
            print("r = \(r), g = \(g), b = \(b)")
            print("width = \(width), height = \(height), bytesPerRow = \(bytesPerRow)")
            
            imageRecognizer.inference(imageBuffer: intBuffer, width: Int32(height), height: Int32(width))
        }
        
    }
    
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        
        print("didReceiveMemoryWarning")
        // Dispose of any resources that can be recreated.
    }
    
    func fakeData() -> Array<SSDData> {
        let obj1 = SSDData.init(with: "Car", accuracy: 0.2, xmin: 0.1, ymin: 0.1, xmax: 0.5, ymax: 0.5, rectSize: self.view.frame.size)
        
        let obj2 = SSDData.init(with: "Cow", accuracy: 0.7, xmin: 0.1, ymin: 0.5, xmax: 0.9, ymax: 0.8, rectSize: self.view.frame.size)
        
        return [obj1, obj2]
    }
}
