//
//  ImageRecognizerPaddleWrapper.m
//  SSDDemo
//
//  Created by Nicky Chan on 11/7/17.
//  Copyright © 2017 PaddlePaddle. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ImageRecognizerPaddleWrapper.h"
#include "paddle_image_recognizer.h"
#include "SSDData.h"

@interface ImageRecognizerPaddleWrapper () {
    ImageRecognizer recognizer;
}
@end

@implementation ImageRecognizerPaddleWrapper

static NSString * kLabels [21] = {
    @"background", @"aeroplane",   @"bicycle", @"background", @"boat",
    @"bottle",     @"bus",         @"car",     @"cat",        @"chair",
    @"cow",        @"diningtable", @"dog",     @"horse",      @"motorbike",
    @"person",     @"pottedplant", @"sheep",   @"sofa",       @"train",
    @"tvmonitor"
};

static float kFilterScore = 0.5;

- (instancetype)init {
    self = [super init];
    if (self)
    {
        int normedHeight = 300;
        int normedWidth  = 300;
        int channel = 3;
        const std::vector<float> means({104, 117, 124});
        
        NSBundle* bundle = [NSBundle mainBundle];
        NSString* resourceDirectoryPath = [bundle bundlePath];
        NSString* path = [resourceDirectoryPath stringByAppendingString: @"/vgg_ssd_net.paddle"];
        
        self->recognizer.init([path UTF8String], normedHeight, normedWidth, channel, means);
        
    }
    return self;
}

- (NSMutableArray*)inference:(unsigned char *)pixels withHeight:(int)height withWidth:(int)width {
    ImageRecognizer::Result result;
    int channel = 4;
    self->recognizer.infer(pixels, height, width, channel, image::CLOCKWISE_R90, result);
    
    NSMutableArray *array = [[NSMutableArray alloc] initWithCapacity:result.height];
    int w = result.width;
    
    for (int i = 0; i < result.height; i++) {
        float score = result.data[i * w + 2];
        if (score < kFilterScore) continue;
        
        SSDData *ssdData = [[SSDData alloc] init];
        ssdData.label = kLabels[(int) result.data[i * w + 1]];
        ssdData.accuracy = score;
        ssdData.xmin = result.data[i * w + 3];
        ssdData.ymin = result.data[i * w + 4];
        ssdData.xmax = result.data[i * w + 5];
        ssdData.ymax = result.data[i * w + 6];
        
        [array addObject:ssdData];
    }
    
    return array;
}

- (void)destroy {
    self->recognizer.release();
}


@end
