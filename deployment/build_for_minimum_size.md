# Build mobile inference library with minimum size

In many mobile applications, the size of the executable program will have some requirements.
Here we explore how to compile the inference library with minimum size.

Note:
In the original PaddlePaddle, all computationally relevant code is implemented in Matrix.cpp and BaseMatrix.cu, 
this causes the compiled Matrix.o and BaseMatrix.o files to be large and can not be split.
The module of Layer can be split, but the Layer forward and backward computing is included in the same file.
The configuration definition in proto has some redundancy. These will lead to the size of inference library larger.

The new PaddlePaddle is being refactored, the calculation is based on Operator, the configuration description in proto is simplified.
These will bring a good optimization to the inference library size.

Here we mainly introduce some optimization in the compilation process to reduce the size of the inference library.

- MinSizeRel: By specify the build type(CMAKE_BUILD_TYPE) as MinSizeRel, the -Os option is used by the compiler during the build for minimum size code.
- protobuf-lite: With the `--cpp_out lite` option, the generated proto configuration rely on MessageLite instead of Message,
so only libprotobuf-lite is needed for the link, and the size of the final executable file can be reduced.
- whole-archive: In `libpaddle_capi_layers.a` contains all the object file of the layers, need use the `--whole-archive` option to ensure that all layers be linked to the executable file.
But don't forget to use `-no-whole-archive after` after `libpaddle_capi_layers.a`, avoid this option affect other libraries.
[Here's an example](https://github.com/PaddlePaddle/Mobile/blob/develop/benchmark/tool/C/CMakeLists.txt#L41)
- Shared library: When building a shared library by `libpaddle_capi_layers.a` and `libpaddle_capi_engine.a`,
you can remove the useless export symbols with the `--version-script` option to reduce the size of the `.dynsym` and `.dynstr` sections.
[Here's an example](https://github.com/PaddlePaddle/Paddle/blob/develop/paddle/capi/CMakeLists.txt#L61)

