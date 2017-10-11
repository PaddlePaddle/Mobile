# Mobile

Here mainly describes how to deploy PaddlePaddle to the mobile end, as well as some deployment optimization methods and some benchmark.

## How to build PaddlePaddle for mobile
- [Build PaddlePaddle for Android](https://github.com/PaddlePaddle/Paddle/blob/develop/doc/howto/cross_compiling/cross_compiling_for_android_cn.md)
- Build PaddlePaddle for IOS
- [Build PaddlePaddle for Raspberry Pi3](https://github.com/PaddlePaddle/Paddle/blob/develop/doc/howto/cross_compiling/cross_compiling_for_raspberry_cn.md)
- Build PaddlePaddle for PX2

## Deployment optimization methods
- [Merge batch normalization before deploying the model to the mobile.](https://github.com/hedaoyuan/Mobile/tree/master/merge_batch_normalization)
- [Compress the model before deploying the model to the mobile.](https://github.com/hedaoyuan/Mobile/tree/master/model_compression/rounding)
- Merge multiple model parameter files into one file.

## Model compression
- Pruning

## PaddlePaddle mobile benchmark
- Mobilenet
- ENet
