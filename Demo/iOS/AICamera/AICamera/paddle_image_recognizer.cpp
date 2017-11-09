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

#include "paddle_image_recognizer.h"
#include <string.h>
#include <iostream>

static void* read_config(const char* filename, long* size) {
  std::cout << "filname = " << filename << std::endl;
  FILE* file = fopen(filename, "rb");
  if (file == NULL) {
    fprintf(stderr, "Open %s error\n", filename);
    return NULL;
  }
  fseek(file, 0L, SEEK_END);
  *size = ftell(file);
  fseek(file, 0L, SEEK_SET);
  void* buf = malloc(*size);
  fread(buf, 1, *size, file);
  fclose(file);
  return buf;
}

void ImageRecognizer::init(const char* merged_model_path) {
  // Step 1: Reading merged model.
  long size;
  void* buf = read_config(merged_model_path, &size);

  // Step 2:
  //    Create a gradient machine for inference.
  CHECK(paddle_gradient_machine_create_for_inference_with_parameters(
      &gradient_machine_, buf, size));
}

void ImageRecognizer::infer(const float* pixels,
                            const size_t height,
                            const size_t width,
                            const size_t channel,
                            Result& result) {
  // Step 3: Prepare input Arguments
  paddle_arguments in_args = paddle_arguments_create_none();

  // There is only one input of this network.
  CHECK(paddle_arguments_resize(in_args, 1));

  // Create input matrix.
  // Set the value
  paddle_matrix mat = paddle_matrix_create(
      /* sample_num */ 1,
      /* size */ channel * height * width,
      /* useGPU */ false);
  CHECK(paddle_arguments_set_value(in_args, 0, mat));

  // Get First row.
  paddle_real* array;
  CHECK(paddle_matrix_get_row(mat, 0, &array));

  memcpy(array, pixels, channel * height * width * sizeof(float));

  // Step 4: Do inference.
  paddle_arguments out_args = paddle_arguments_create_none();
  {
    CHECK(paddle_gradient_machine_forward(gradient_machine_,
                                          in_args,
                                          out_args,
                                          /* isTrain */ false));
  }

  // Step 5: Get the result.
  paddle_matrix probs = paddle_matrix_create_none();
  CHECK(paddle_arguments_get_value(out_args, 0, probs));

  CHECK(paddle_matrix_get_shape(probs, &result.height, &result.width));
  CHECK(paddle_matrix_get_row(probs, 0, &result.data));

  // Step 6: Release the resources.
  CHECK(paddle_arguments_destroy(in_args));
  CHECK(paddle_matrix_destroy(mat));
  CHECK(paddle_arguments_destroy(out_args));
  CHECK(paddle_matrix_destroy(probs));
}

void ImageRecognizer::release() {
  if (gradient_machine_ != nullptr) {
    CHECK(paddle_gradient_machine_destroy(gradient_machine_));
  }
}
