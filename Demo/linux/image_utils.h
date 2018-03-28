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

#include <stdio.h>
#include <vector>
#include "image.h"

namespace image {
namespace utils {

void resize_hwc(const unsigned char* pixels,
                unsigned char* resized_pixels,
                const size_t height,
                const size_t width,
                const size_t channel,
                const size_t resized_height,
                const size_t resized_width);

void rotate_hwc(const unsigned char* pixels,
                unsigned char* rotated_pixels,
                const size_t height,
                const size_t width,
                const size_t channel,
                const RotateOption option);

void normalize(const unsigned char* pixels,
               float* normed_pixels,
               const size_t height,
               const size_t width,
               const size_t channel,
               const size_t normed_height,
               const size_t normed_width,
               const size_t normed_channel,
               const std::vector<float>& means,
               const image::Config& config);

}  // namespace utils
}  // namespace image
