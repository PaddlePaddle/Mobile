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

#include "image_reader.h"
#include <iostream>
#include <vector>

#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>

bool ImageReader::operator()(const std::string& image_path,
                             float* data,
                             const size_t height,
                             const size_t width,
                             const size_t channel,
                             const Order order) {
  if (data == NULL || image_path.empty() || channel != means_.size()) {
    std::cerr << "invalid arguments." << std::endl;
    return false;
  }

  cv::Mat image;
  if (1) {
    image = cv::imread(image_path, CV_LOAD_IMAGE_COLOR);
  } else {
    image = cv::imread(image_path, CV_LOAD_IMAGE_GRAYSCALE);
  }

  if (image.empty()) {
    std::cerr << "image is empty." << std::endl;
    return false;
  }

  size_t image_width = static_cast<size_t>(image.cols);
  size_t image_height = static_cast<size_t>(image.rows);
  size_t image_channel = static_cast<size_t>(image.channels());
  if (image_width != width || image_height != height) {
    std::cerr << "the size of image does not match the network: "
              << image_height << " vs. " << height << ", " << image_width
              << " vs. " << width << std::endl;
    return false;
  }

  if (channel == 3) {
    size_t index = 0;
    if (order == kCHW) {
      // Store the pixels in CHW, BGR order
      for (size_t c = 0; c < channel; c++) {
        for (size_t y = 0; y < height; y++) {
          for (size_t x = 0; x < width; x++) {
            data[index] =
                static_cast<float>(image.at<cv::Vec3b>(y, x)[c]) - means_[c];
            index++;
          }
        }
      }
    } else /* kHWC */ {
      // Store the pixels in HWC, BGR order
      for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
          for (size_t c = 0; c < channel; c++) {
            data[index] =
                static_cast<float>(image.at<cv::Vec3b>(y, x)[c]) - means_[c];
            index++;
          }
        }
      }
    }
  } else /* gray-scale */ {
    size_t index = 0;
    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < width; x++) {
        data[index] =
            static_cast<float>(image.at<unsigned char>(y, x)) - means_[0];
        index++;
      }
    }
  }
  return true;
}
#else
bool ImageReader::operator()(const std::string& image_path,
                             float* data,
                             const size_t height,
                             const size_t width,
                             const size_t channel,
                             const Order order) {
  if (data == NULL || height <= 0 || width <= 0 || channel != means_.size()) {
    std::cerr << "invalid arguments." << std::endl;
    return false;
  }

  size_t index = 0;
  if (order == kCHW) {
    for (size_t c = 0; c < channel; ++c) {
      for (size_t h = 0; h < height; ++h) {
        for (size_t w = 0; w < width; ++w) {
          data[index] = index % 255 - means_[c];
          index++;
        }
      }
    }
  } else /* kHWC */ {
    for (size_t h = 0; h < height; ++h) {
      for (size_t w = 0; w < width; ++w) {
        for (size_t c = 0; c < channel; ++c) {
          data[index] = index % 255 - means_[c];
          index++;
        }
      }
    }
  }
  return true;
}
#endif
