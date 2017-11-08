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

#include <string>
#include <vector>

enum Order {
  kHWC = 0,
  kCHW = 1,
};

class ImageReader {
public:
  ImageReader(const std::vector<float>& means) : means_(means) {}

  bool operator()(const std::string& image_path,
                  float* data,
                  const size_t height,
                  const size_t width,
                  const size_t channel,
                  const Order order);

private:
  std::vector<float> means_;
};
