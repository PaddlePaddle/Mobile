# Inference demo

This is an inference demo program based on the C API of PaddlePaddle.
The demo explained here is based on the C++ code, so we need to use g++ or clang++ to compile.
The demo can be run from the command line and can be used to test the inference performance of various different models.

## Android
To compile and run this demo in an Android environment, please follow the following steps:

1. Refer to [this document](https://github.com/PaddlePaddle/Paddle/blob/develop/doc/howto/cross_compiling/cross_compiling_for_android_cn.md) to compile the Android version of PaddlePaddle. After following the mentioned steps, make install will generate an output directory containing three subdirectories: include, lib, and third_party.
2. Compile `inference.cc` to an executable program for the Android environment as follows:
    - For armeabi-v7a
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
    - For arm64-v8a
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
3. Run the demo program by logging into the Android environment via adb and specifying the PaddlePaddle model from the command line as follows:
```
./inference --merged_model ./model/mobilenet.paddle --input_size 150528
```
