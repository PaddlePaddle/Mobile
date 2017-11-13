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
#include "image_utils.h"
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

void draw_rectangles(const float* raw_pixels,
                     float* rected_pixels,
                     const size_t height,
                     const size_t width,
                     const size_t channel) {}

void test_noresize(ImageRecognizer& recognizer,
                   const size_t kImageHeight,
                   const size_t kImageWidth,
                   const size_t kImageChannel,
                   ImageRecognizer::Result& result) {
  // Read RGB data from image
  unsigned char* raw_pixels = (unsigned char*)malloc(
      kImageHeight * kImageWidth * kImageChannel * sizeof(unsigned char));
  ImageReader()("images/resized.jpg",
                raw_pixels,
                kImageHeight,
                kImageWidth,
                kImageChannel,
                kHWC);

  recognizer.infer(raw_pixels,
                   kImageHeight,
                   kImageWidth,
                   kImageChannel,
                   image::NO_ROTATE,
                   result);

  free(raw_pixels);
  raw_pixels = nullptr;
}

void test_resize(ImageRecognizer& recognizer,
                 const size_t kImageHeight,
                 const size_t kImageWidth,
                 const size_t kImageChannel,
                 ImageRecognizer::Result& result) {
  const size_t height = 500;
  const size_t width = 353;
  const size_t channel = 3;

  // Read RGB data from image
  unsigned char* raw_pixels =
      (unsigned char*)malloc(height * width * channel * sizeof(unsigned char));
  ImageReader()("images/origin.jpg", raw_pixels, height, width, channel, kHWC);

  recognizer.infer(
      raw_pixels, height, width, channel, image::NO_ROTATE, result);

  free(raw_pixels);
  raw_pixels = nullptr;
}

void test_rgba(ImageRecognizer& recognizer,
               const size_t kImageHeight,
               const size_t kImageWidth,
               const size_t kImageChannel,
               ImageRecognizer::Result& result) {
  const size_t height = 500;
  const size_t width = 353;
  const size_t channel = 3;

  // Read RGB data from image
  unsigned char* raw_pixels =
      (unsigned char*)malloc(height * width * channel * sizeof(unsigned char));
  ImageReader()("images/origin.jpg", raw_pixels, height, width, channel, kHWC);

  // Padding to RGBA, for testing
  // Only RGB is needed
  const size_t channel_rgba = 4;
  unsigned char* pixels = (unsigned char*)malloc(height * width * channel_rgba *
                                                 sizeof(unsigned char));
  for (size_t i = 0; i < height * width; ++i) {
    pixels[i * channel_rgba + 0] = raw_pixels[i * channel + 0];
    pixels[i * channel_rgba + 1] = raw_pixels[i * channel + 1];
    pixels[i * channel_rgba + 2] = raw_pixels[i * channel + 2];
    pixels[i * channel_rgba + 3] = 0;  // alpha
  }

  recognizer.infer(
      pixels, height, width, channel_rgba, image::NO_ROTATE, result);

  free(raw_pixels);
  raw_pixels = nullptr;
  free(pixels);
  pixels = nullptr;
}

void test_rotate(ImageRecognizer& recognizer,
                 const size_t kImageHeight,
                 const size_t kImageWidth,
                 const size_t kImageChannel,
                 ImageRecognizer::Result& result) {
  const size_t height = 353;
  const size_t width = 500;
  const size_t channel = 3;

  // Read RGB data from image
  unsigned char* raw_pixels =
      (unsigned char*)malloc(height * width * channel * sizeof(unsigned char));
  ImageReader()("images/rotated.jpg", raw_pixels, height, width, channel, kHWC);

  recognizer.infer(
      raw_pixels, height, width, channel, image::CLOCKWISE_R90, result);

  free(raw_pixels);
  raw_pixels = nullptr;
}

int main() {
  ImageRecognizer::init_paddle();

#if 1
  const char* merged_model_path = "models/vgg_ssd_net.paddle";

  const size_t kImageHeight = 300;
  const size_t kImageWidth = 300;
  const size_t kImageChannel = 3;
#else
  const char* merged_model_path = "models/mobilenet_ssd_160.paddle";

  const size_t kImageHeight = 160;
  const size_t kImageWidth = 160;
  const size_t kImageChannel = 3;
#endif

  const std::vector<float> means({104, 117, 124});

  ImageRecognizer recognizer;
  recognizer.init(
      merged_model_path, kImageHeight, kImageWidth, kImageChannel, means);

  ImageRecognizer::Result result;
  test_rotate(recognizer, kImageHeight, kImageWidth, kImageChannel, result);

  // Print the direct result
  std::cout << "Direct Result: " << result.height << " x " << result.width
            << std::endl;
  for (uint64_t i = 0; i < result.height; i++) {
    std::cout << "row " << i << ":";
    for (uint64_t j = 0; j < result.width; j++) {
      std::cout << " " << result.data[i * result.width + j];
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  // Print the profiled result
  std::cout << "Profiled result" << std::endl;
  profile(result, /* threshold */ 0);

  // You may need to use a threshold to filter out objects with low score
  std::cout << "Profiled result (threshold = 0.3)" << std::endl;
  profile(result, /* threshold */ 0.3);

  // Draw rectangles
  // float* rected_pixels = (float*)malloc(kImageHeight * kImageWidth *
  // kImageChannel *
  //                                sizeof(float));
  // ImageWriter()("images/result.jpg", raw_pixels, kImageHeight, kImageWidth,
  // kImageChannel, kCHW);

  recognizer.release();

  return 0;
}
