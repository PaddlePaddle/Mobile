/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserve.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License */

#pragma once

#include <paddle/capi.h>
#include <stdio.h>
#include <stdlib.h>

static const char* paddle_error_string(paddle_error status) {
  switch (status) {
    case kPD_NULLPTR:
      return "nullptr error";
    case kPD_OUT_OF_RANGE:
      return "out of range error";
    case kPD_PROTOBUF_ERROR:
      return "protobuf error";
    case kPD_NOT_SUPPORTED:
      return "not supported error";
    case kPD_UNDEFINED_ERROR:
      return "undefined error";
  };
}

#define CHECK(stmt)                                            \
  do {                                                         \
    paddle_error __err__ = stmt;                               \
    if (__err__ != kPD_NO_ERROR) {                             \
      const char* str = paddle_error_string(__err__);          \
      fprintf(stderr, "%s (%d) in " #stmt "\n", str, __err__); \
      exit(__err__);                                           \
    }                                                          \
  } while (0)

class ImageRecognizer {
public:
  struct Result {
    Result() : data(nullptr), height(0), width(0) {}

    float* data;
    uint64_t height;
    uint64_t width;
  };

public:
  ImageRecognizer() : gradient_machine_(nullptr) {
    // Initalize Paddle
    char* argv[] = {const_cast<char*>("--use_gpu=False")};
    CHECK(paddle_init(1, (char**)argv));
  }

  void init(const char* merged_model_path);
  void infer(const float* pixels,
             const size_t height,
             const size_t width,
             const size_t channel,
             Result& result);
  void release();

private:
  paddle_gradient_machine gradient_machine_;
  paddle_error error_;
};
