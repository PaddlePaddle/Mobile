# Inference demo

This is an inference demo program based on the Paddle C API. But this demo is based on the c++ code, so need to use g++ or clang++ to compile. 

## Android
To compile and run this demo in the Android environment, follow these steps:

1. Refer to [this document](https://github.com/PaddlePaddle/Paddle/blob/develop/doc/howto/cross_compiling/cross_compiling_for_android_cn.md) to compile the paddle of android version.
2. Compile this inference.cc to an executable program for the Android environment.
3. Run the demo program by logging into the Android environment via adb and specifying the paddle model from the command line.
```
./inference --merged_model ./model/mobilenet.paddle --input_size 150528
```
