# Inference demo

This is an inference demo program based on the Paddle C API. But this demo is based on the c++ code, so need to use g++ or clang++ to compile. 
The demo can be run from the command line and used to test the inference performance of various models.

## Android
To compile and run this demo in the Android environment, follow these steps:

1. Refer to [this document](https://github.com/PaddlePaddle/Paddle/blob/develop/doc/howto/cross_compiling/cross_compiling_for_android_cn.md) to compile the paddle of android version. After executing make install will generate an output directory containing three subdirectories of include, lib, and third_party.
2. Compile this inference.cc to an executable program for the Android environment as follow.
    - armeabi-v7a
    ```
    mkdir build
    cd build

    cmake .. \
    -DANDROID_ABI=armeabi-v7a \
    -DANDROID_STANDALONE_TOOLCHAIN=your/path/to/arm_standalone_toolchain \
    -DPADDLE_ROOT=The output path generated in the first step \
    -DCMAKE_BUILD_TYPE=MinSizeRel

    make
    ```
    - arm64-v8a
    ```
    mkdir build
    cd build

    cmake .. \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_STANDALONE_TOOLCHAIN=your/path/to/arm64_standalone_toolchain \
    -DPADDLE_ROOT=The output path generated in the first step \
    -DCMAKE_BUILD_TYPE=MinSizeRel

    make
    ```
3. Run the demo program by logging into the Android environment via adb and specifying the paddle model from the command line.
```
./inference --merged_model ./model/mobilenet.paddle --input_size 150528
```
