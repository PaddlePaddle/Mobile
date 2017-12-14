# Mobile

[![Build Status](https://travis-ci.org/PaddlePaddle/Mobile.svg?branch=develop)](https://travis-ci.org/PaddlePaddle/Mobile)
[![License](https://img.shields.io/badge/license-Apache%202-blue.svg)](LICENSE)

Here mainly describes how to deploy PaddlePaddle to the mobile end, as well as some deployment optimization methods and some benchmark.

## How to build PaddlePaddle for mobile
- Build PaddlePaddle for Android [[Chinese](https://github.com/PaddlePaddle/Paddle/blob/develop/doc/mobile/cross_compiling_for_android_cn.md)] [[English](https://github.com/PaddlePaddle/Paddle/blob/develop/doc/mobile/cross_compiling_for_android_en.md)]
- Build PaddlePaddle for IOS [[Chinese](https://github.com/PaddlePaddle/Paddle/blob/develop/doc/mobile/cross_compiling_for_ios_cn.md)]
- Build PaddlePaddle for Raspberry Pi3 [[Chinese](https://github.com/PaddlePaddle/Paddle/blob/develop/doc/mobile/cross_compiling_for_raspberry_cn.md)] [[English](https://github.com/PaddlePaddle/Paddle/blob/develop/doc/mobile/cross_compiling_for_raspberry_en.md)]
- Build PaddlePaddle for PX2
- [How to build PaddlePaddle mobile inference library with minimum size.](./deployment/library/build_for_minimum_size.md)

## Demo
- [A command-line inference demo.](./benchmark/tool/C/README.md)
- [iOS demo of AICamera](./Demo/iOS/AICamera/README.md)

## Deployment optimization methods
- [Merge batch normalization before deploying the model to the mobile.](./deployment/model/merge_batch_normalization/README.md)
- [Compress the model before deploying the model to the mobile.](./deployment/model/rounding/README.md)
- [Merge model config and parameter files into one file.](./deployment/model/merge_config_parameters/README.md)
- How to deploy int8 model in mobile inference with PaddlePaddle.

## Model compression
- [How to use pruning to train smaller model](./model_compression/pruning/)

## PaddlePaddle mobile benchmark
- [Benchmark of Mobilenet](./benchmark/README.md)
- Benchmark of ENet
- [Benchmark of DepthwiseConvolution in PaddlePaddle](https://github.com/hedaoyuan/Function/blob/master/src/conv/README.md)

This tutorial is contributed by [PaddlePaddle](https://github.com/PaddlePaddle/Paddle) and licensed under the [Apache-2.0 license](LICENSE).
