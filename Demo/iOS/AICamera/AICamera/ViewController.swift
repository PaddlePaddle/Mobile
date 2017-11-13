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
    var multiboxLayer : SSDMultiboxLayer?
    var previewLayer : AVCaptureVideoPreviewLayer?
    var videoConnection: AVCaptureConnection!
    var captureDevice : AVCaptureDevice?
    
    let imageRecognizer = ImageRecognizer()
    
    var index = 0
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        
        captureSession.sessionPreset = AVCaptureSessionPresetHigh
        
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
//        previewLayer.frame = CGRect(x: 0, y: 0, width: self.view.frame.width, height: self.view.frame.width)
        previewLayer.frame = previewContainer.bounds
        previewLayer.contentsGravity = kCAGravityResizeAspect
        previewLayer.videoGravity = AVLayerVideoGravityResizeAspect
//        previewLayer.connection.videoOrientation = .portrait
        previewContainer.insertSublayer(previewLayer, at: 0)
        self.previewLayer = previewLayer
        
        multiboxLayer = SSDMultiboxLayer()
        previewContainer.insertSublayer(multiboxLayer!, at: 1)
        
        // setup video output
        do {
            let videoDataOutput = AVCaptureVideoDataOutput()
            videoDataOutput.videoSettings = [kCVPixelBufferPixelFormatTypeKey: Int(kCVPixelFormatType_32BGRA)]
            videoDataOutput.alwaysDiscardsLateVideoFrames = true
            guard captureSession.canAddOutput(videoDataOutput) else {
                fatalError("CaptureSession can not add output")
            }
            captureSession.addOutput(videoDataOutput)
            
            captureSession.commitConfiguration()
            
            let queue = DispatchQueue(label: "com.paddlepaddle.SSDDemo")
            videoDataOutput.setSampleBufferDelegate(self, queue: queue)
            
            if let connection = videoDataOutput.connection(withMediaType: AVMediaTypeVideo) {
                if connection.isVideoOrientationSupported {
                    print("orientation support")
                    // Force recording to portrait
//                    connection.videoOrientation = .portrait
                }
                self.videoConnection = connection
            }
            captureSession.startRunning()
        }
    }
    
    func captureOutput(_ output: AVCaptureOutput, didDrop sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {
//        print("didDrop")
    }
    
    func captureOutput(_ output: AVCaptureOutput, didOutputSampleBuffer sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {
        
        NSLog("didOutput \(index)")
        
        index = index + 1
        if let imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer) {
            CVPixelBufferLockBaseAddress(imageBuffer, CVPixelBufferLockFlags(rawValue: 0))
        
            let width = CVPixelBufferGetWidth(imageBuffer)
            let height = CVPixelBufferGetHeight(imageBuffer)
            let baseAddress = CVPixelBufferGetBaseAddress(imageBuffer)
            
            let intBuffer = unsafeBitCast(baseAddress, to: UnsafeMutablePointer<UInt8>.self)
            
//            CVPixelBufferUnlockBaseAddress(imageBuffer, CVPixelBufferLockFlags(rawValue: 0))
            
            let ssdDataList = imageRecognizer.inference(imageBuffer: intBuffer, width: Int32(width), height: Int32(height))
            print("width = \(width) height =\(height) count =\(ssdDataList!.count)")
            
            DispatchQueue.main.async {
                self.multiboxLayer?.displayBoxs(with: ssdDataList!)
            }
        }
    }
    
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        
        print("didReceiveMemoryWarning")
        // Dispose of any resources that can be recreated.
    }
    
}
