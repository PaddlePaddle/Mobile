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

#include <string.h>
#include <iostream>
#include <vector>
#include "image_io.h"
#ifdef USE_PADDLE_FLUID
#include "paddle_fluid_image_recognizer.h"
#else
#include "paddle_image_recognizer.h"
#endif

void test_normal(ImageRecognizer& recognizer,
                 const size_t kImageHeight,
                 const size_t kImageWidth,
                 const size_t kImageChannel,
                 ImageRecognizer::Result& result) {
  // Read BGR data from image
  unsigned char* raw_pixels = (unsigned char*)malloc(
      kImageHeight * kImageWidth * kImageChannel * sizeof(unsigned char));
  image::io::ImageReader()("image_classification/images/chicken.jpg",
                           raw_pixels,
                           kImageHeight,
                           kImageWidth,
                           kImageChannel,
                           image::kHWC);

  image::Config config(image::kRGB, image::NO_ROTATE);
  recognizer.infer(
      raw_pixels, kImageHeight, kImageWidth, kImageChannel, config, result);

  free(raw_pixels);
  raw_pixels = nullptr;
}

int main() {
  ImageRecognizer::init_paddle();

  const size_t kImageHeight = 224;
  const size_t kImageWidth = 224;
  const size_t kImageChannel = 3;

  const std::vector<float> means({103.939, 116.779, 123.680});

  ImageRecognizer recognizer;

#ifdef USE_PADDLE_FLUID
  std::string dirname = "image_classification/models/fluid/resnet50";
  std::string model_path = dirname + "/model";
  std::string params_path = dirname + "/params";

  recognizer.init(
      model_path, params_path, kImageHeight, kImageWidth, kImageChannel, means);
#else
  const char* merged_model_path =
      "image_classification/models/v2/resnet_50.paddle";
  recognizer.init(
      merged_model_path, kImageHeight, kImageWidth, kImageChannel, means);
#endif

  ImageRecognizer::Result result;
  test_normal(recognizer, kImageHeight, kImageWidth, kImageChannel, result);

  // Print the direct result
  std::cout << "Direct Result: " << result.height << " x " << result.width
            << std::endl;
  for (uint64_t i = 0; i < result.height; i++) {
    std::cout << i << "-th: ";
    float max_probs = result.data[i * result.width];
    uint64_t max_id = 0;
    for (uint64_t j = 0; j < result.width; j++) {
      if (result.data[i * result.width + j] > max_probs) {
        max_probs = result.data[i * result.width + j];
        max_id = j;
      }
    }
    std::cout << "Type: " << max_id << ", Probs: " << max_probs;
  }
  std::cout << std::endl;

  recognizer.release();

  return 0;
}
