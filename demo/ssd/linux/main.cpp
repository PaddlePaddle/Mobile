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

#include <stdio.h>
#include <vector>
#include "image_reader.h"
#include "paddle_image_recognizer.h"

int main() {
  const char* merged_model_path = "models/vgg_ssd_net.paddle";

  ImageRecognizer recognizer;
  recognizer.init(merged_model_path);

  const size_t kImageHeight = 300;
  const size_t kImageWidth = 300;
  const size_t kImageChannel = 3;

  float* pixels = (float*)malloc(kImageHeight * kImageWidth * kImageChannel *
                                 sizeof(float));

  std::vector<float> means({104, 117, 124});
  ImageReader reader(means);
  reader("images/example.jpg",
         pixels,
         kImageHeight,
         kImageWidth,
         kImageChannel,
         kCHW);

  ImageRecognizer::Result result;
  recognizer.infer(pixels, kImageHeight, kImageWidth, kImageChannel, result);

  printf("Prob: %lld x %lld\n", result.height, result.width);
  for (uint64_t i = 0; i < result.height; i++) {
    printf("row %d: ", i);
    for (uint64_t j = 0; j < result.width; j++) {
      printf("%f ", result.data[i * result.width + j]);
    }
    printf("\n");
  }

  recognizer.release();

  return 0;
}
