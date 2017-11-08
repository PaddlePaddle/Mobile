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

#include <iostream>
#include <vector>
#include "image_reader.h"
#include "paddle_image_recognizer.h"

void profile(ImageRecognizer::Result& result, float threshold) {
  std::string labels[21] = {
      "background", "aeroplane",   "bicycle", "bird",  "boat",
      "bottle",     "bus",         "car",     "cat",   "chair",
      "cow",        "diningtable", "dog",     "horse", "motorbike",
      "person",     "pottedplant", "sheep",   "sofa",  "train",
      "tvmonitor"};

  for (uint64_t i = 0; i < result.height; i++) {
    if (result.width == 7UL && result.data[i * result.width + 2] >= threshold) {
      std::cout << "Object " << i << std::endl;
      std::cout << "\timage: "
                << static_cast<int>(result.data[i * result.width + 0])
                << std::endl;
      std::cout << "\ttype: "
                << labels[static_cast<int>(result.data[i * result.width + 1])]
                << std::endl;
      std::cout << "\tscore: " << result.data[i * result.width + 2]
                << std::endl;
      std::cout << "\trectangle information:" << std::endl;
      std::cout << "\t\txmin, " << result.data[i * result.width + 3]
                << std::endl;
      std::cout << "\t\tymin, " << result.data[i * result.width + 4]
                << std::endl;
      std::cout << "\t\txmax, " << result.data[i * result.width + 5]
                << std::endl;
      std::cout << "\t\tymax, " << result.data[i * result.width + 6]
                << std::endl;
    }
  }
  std::cout << std::endl;
}

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

  std::cout << "Result: " << result.height << " x " << result.width
            << std::endl;
  for (uint64_t i = 0; i < result.height; i++) {
    std::cout << "row " << i << ":";
    for (uint64_t j = 0; j < result.width; j++) {
      std::cout << " " << result.data[i * result.width + j];
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  std::cout << "Profiled result" << std::endl;
  profile(result, /* threshold */ 0);

  // You may need to use a threshold to filter out objects with low score
  std::cout << "Profiled result (threshold = 0.3)" << std::endl;
  profile(result, /* threshold */ 0.3);

  recognizer.release();

  free(pixels);
  pixels = nullptr;

  return 0;
}
