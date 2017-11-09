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

@interface ImageRecognizerPaddleWrapper () {
    ImageRecognizer recognizer;
}
@end

@implementation ImageRecognizerPaddleWrapper


- (id)init {
    self = [super init];
    if (self)
    {
        NSBundle* bundle = [NSBundle mainBundle];
        NSString* resourceDirectoryPath = [bundle bundlePath];
        NSString* path = [resourceDirectoryPath stringByAppendingString: @"/vgg_ssd_net.paddle"];
        self->recognizer.init([path UTF8String]);
    }
    return self;
}

- (void)inference {
    
}

- (void)destroy {
    self->recognizer.release();
}


@end
