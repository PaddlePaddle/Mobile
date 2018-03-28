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

static void* read_config(const char* filename, long* size) {
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

void ImageRecognizer::init(const char* merged_model_path,
                           const size_t normed_height,
                           const size_t normed_width,
                           const size_t normed_channel,
                           const std::vector<float>& means) {
  // Set the normed image size
  normed_height_ = normed_height;
  normed_width_ = normed_width;
  normed_channel_ = normed_channel;

  // Set means
  if (!means.empty()) {
    means_ = means;
  } else {
    means_.clear();
    for (size_t i = 0; i < normed_channel; ++i) {
      means_.push_back(0.0f);
    }
  }

  // Step 1: Reading merged model.
  long size;
  void* buf = read_config(merged_model_path, &size);

  // Step 2:
  //    Create a gradient machine for inference.
  CHECK(paddle_gradient_machine_create_for_inference_with_parameters(
      &gradient_machine_, buf, size));

  free(buf);
  buf = nullptr;
}

void ImageRecognizer::infer(const unsigned char* pixels,
                            const size_t height,
                            const size_t width,
                            const size_t channel,
                            const image::Config& config,
                            Result& result) {
  if (height < normed_height_ || width < normed_width_) {
    fprintf(stderr,
            "Image size should be no less than the normed size (%u vs %u, %u "
            "vs %u).\n",
            height,
            normed_height_,
            width,
            normed_width_);
    return;
  }

  // Step 3: Prepare input Arguments
  paddle_arguments in_args = paddle_arguments_create_none();

  // There is only one input of this network.
  CHECK(paddle_arguments_resize(in_args, 1));

  // Create input matrix.
  // Set the value
  paddle_matrix mat = paddle_matrix_create(
      /* sample_num */ 1,
      /* size */ normed_channel_ * normed_height_ * normed_width_,
      /* useGPU */ false);
  CHECK(paddle_arguments_set_value(in_args, 0, mat));

  // Get First row.
  paddle_real* array;
  CHECK(paddle_matrix_get_row(mat, 0, &array));

  image::utils::normalize(pixels,
                          array,
                          height,
                          width,
                          channel,
                          normed_height_,
                          normed_width_,
                          normed_channel_,
                          means_,
                          config);

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

  paddle_error err = paddle_matrix_get_row(probs, 0, &result.data);
  if (err == kPD_NO_ERROR) {
    CHECK(paddle_matrix_get_shape(probs, &result.height, &result.width));
  }

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
