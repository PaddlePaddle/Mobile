//
//  ImageRecognizerPaddleWrapper.h
//  SSDDemo
//
//  Created by Nicky Chan on 11/7/17.
//  Copyright © 2017 PaddlePaddle. All rights reserved.
//


#import <Foundation/Foundation.h>

@interface ImageRecognizerPaddleWrapper : NSObject

- (void)initialize;
- (void)inference;
- (void)unbind;

@end


