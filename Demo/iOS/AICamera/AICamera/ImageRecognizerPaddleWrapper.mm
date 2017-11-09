//
//  ImageRecognizerPaddleWrapper.m
//  SSDDemo
//
//  Created by Nicky Chan on 11/7/17.
//  Copyright © 2017 PaddlePaddle. All rights reserved.
//

#include "paddle_image_recognizer.h"
#include "ImageRecognizerPaddleWrapper.h"

@implementation ImageRecognizerPaddleWrapper

- (id)init {
    return self;
}

- (void)initialize {
    ImageRecognizer recognizer;
    recognizer.init("../models/vgg_ssd_net.paddle");
}
- (void)inference {
    
}

- (void)unbind {
    
}
@end
