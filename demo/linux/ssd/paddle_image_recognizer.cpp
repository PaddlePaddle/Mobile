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

namespace image {

void resize_hwc(const unsigned char* pixels,
                unsigned char* resized_pixels,
                const size_t height,
                const size_t width,
                const size_t channel,
                const size_t resized_height,
                const size_t resized_width) {
  float ratio_x = static_cast<float>(width) / static_cast<float>(resized_width);
  float ratio_y =
      static_cast<float>(height) / static_cast<float>(resized_height);

  for (size_t i = 0; i < resized_height; i++) {
    float new_y = i * ratio_y;

    size_t y1 = (static_cast<size_t>(new_y) > (height - 1))
                    ? (height - 1)
                    : static_cast<size_t>(new_y);
    size_t y2 = y1 + 1;

    float b1 = y2 - new_y;
    float b2 = new_y - y1;

    for (size_t j = 0; j < resized_width; j++) {
      float new_x = j * ratio_x;

      size_t x1 = (static_cast<size_t>(new_x) > (width - 1))
                      ? (width - 1)
                      : static_cast<size_t>(new_x);
      int x2 = x1 + 1;

      float a1 = x2 - new_x;
      float a2 = new_x - x1;

      unsigned char* pt_dst =
          resized_pixels + (i * resized_width + j) * channel;
      const unsigned char* pt_src = pixels + (y1 * width + x1) * channel;
      int p1 = width * channel;
      int p2 = p1 + channel;

      if (x1 == width - 1 && y1 == height - 1) {
        memcpy(pt_dst, pt_src, channel * sizeof(unsigned char));
      } else if (x1 == width - 1) {
        for (size_t k = 0; k < channel; k++) {
          float pixel_00 = static_cast<float>(pt_src[k]);
          float pixel_10 = static_cast<float>(pt_src[p1 + k]);

          pt_dst[k] = static_cast<unsigned char>(pixel_00 * b1 + pixel_10 * b2);
        }
      } else if (y1 == height - 1) {
        for (size_t k = 0; k < channel; k++) {
          float pixel_00 = static_cast<float>(pt_src[k]);
          float pixel_01 = static_cast<float>(pt_src[channel + k]);

          pt_dst[k] = static_cast<unsigned char>(pixel_00 * a1 + pixel_01 * a2);
        }
      } else {
        // If x1 = width - 1 or y1 = height - 1, the memory accesses may be out
        // of range.
        for (size_t k = 0; k < channel; k++) {
          float pixel_00 = static_cast<float>(pt_src[k]);
          float pixel_01 = static_cast<float>(pt_src[channel + k]);
          float pixel_10 = static_cast<float>(pt_src[p1 + k]);
          float pixel_11 = static_cast<float>(pt_src[p2 + k]);

          pt_dst[k] =
              static_cast<unsigned char>((pixel_00 * a1 + pixel_01 * a2) * b1 +
                                         (pixel_10 * a1 + pixel_11 * a2) * b2);
        }
      }
    }  // j-loop
  }    // i-loop
}

void rotate_hwc(const unsigned char* pixels,
                unsigned char* rotated_pixels,
                const size_t height,
                const size_t width,
                const size_t channel,
                const RotateOption option) {
  switch (option) {
    case NO_ROTATE:
      memcpy(rotated_pixels,
             pixels,
             height * width * channel * sizeof(unsigned char));
      break;
    case CLOCKWISE_R90:
      for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
          // (i, j) -> (j, height - 1 - i)
          for (size_t k = 0; k < channel; ++k) {
            rotated_pixels[(j * height + height - 1 - i) * channel + k] =
                pixels[(i * width + j) * channel + k];
          }
        }
      }
      break;
    case CLOCKWISE_R180:
      for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
          // (i, j) -> (height - 1 - i, width - 1 - j)
          for (size_t k = 0; k < channel; ++k) {
            rotated_pixels[((height - 1 - i) * width + width - 1 - j) *
                               channel +
                           k] = pixels[(i * width + j) * channel + k];
          }
        }
      }
      break;
    case CLOCKWISE_R270:
      for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
          // (i, j) -> (width - 1 - j, i)
          for (size_t k = 0; k < channel; ++k) {
            rotated_pixels[((width - 1 - j) * height + i) * channel + k] =
                pixels[(i * width + j) * channel + k];
          }
        }
      }
      break;
    default:
      fprintf(stderr,
              "Illegal rotate option, please specify among [NO_ROTATE, "
              "CLOCKWISE_R90, CLOCKWISE_R180, CLOCKWISE_R270].\n");
  }
}

}  // namespace image

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

void ImageRecognizer::preprocess(const unsigned char* pixels,
                                 float* normed_pixels,
                                 const size_t height,
                                 const size_t width,
                                 const size_t channel,
                                 const image::Config& config) {
  bool need_resize = true;
  size_t resized_height = 0;
  size_t resized_width = 0;
  if (config.option == image::NO_ROTATE ||
      config.option == image::CLOCKWISE_R180) {
    if (height == normed_height_ && width == normed_width_) {
      need_resize = false;
    }
    resized_height = normed_height_;
    resized_width = normed_width_;
  } else if (config.option == image::CLOCKWISE_R90 ||
             config.option == image::CLOCKWISE_R270) {
    if (height == normed_width_ && width == normed_height_) {
      need_resize = false;
    }
    resized_height = normed_width_;
    resized_width = normed_height_;
  }

  unsigned char* resized_pixels = nullptr;
  if (!need_resize) {
    resized_pixels = const_cast<unsigned char*>(pixels);
  } else {
    // Bilinear Interpolation Resize
    resized_pixels = static_cast<unsigned char*>(malloc(
        resized_height * resized_width * channel * sizeof(unsigned char)));
    image::resize_hwc(pixels,
                      resized_pixels,
                      height,
                      width,
                      channel,
                      resized_height,
                      resized_width);
  }

  unsigned char* rotated_pixels = nullptr;
  if (config.option == image::NO_ROTATE) {
    rotated_pixels = resized_pixels;
  } else {
    rotated_pixels = static_cast<unsigned char*>(malloc(
        normed_height_ * normed_width_ * channel * sizeof(unsigned char)));
    image::rotate_hwc(resized_pixels,
                      rotated_pixels,
                      resized_height,
                      resized_width,
                      channel,
                      config.option);
  }

  if (true) {
    // HWC -> CHW
    size_t index = 0;
    if (config.format == image::kRGB) {
      // RGB/RGBA -> RGB
      for (size_t c = 0; c < normed_channel_; ++c) {
        for (size_t h = 0; h < normed_height_; ++h) {
          for (size_t w = 0; w < normed_width_; ++w) {
            normed_pixels[index] =
                static_cast<float>(
                    rotated_pixels[(h * normed_width_ + w) * channel + c]) -
                means_[c];
            index++;
          }
        }
      }
    } else if (config.format == image::kBGR) {
      // BGR/BGRA -> RGB
      for (size_t c = 0; c < normed_channel_; ++c) {
        for (size_t h = 0; h < normed_height_; ++h) {
          for (size_t w = 0; w < normed_width_; ++w) {
            normed_pixels[index] =
                static_cast<float>(
                    rotated_pixels[(h * normed_width_ + w) * channel +
                                   (normed_channel_ - 1 - c)]) -
                means_[c];
            index++;
          }
        }
      }
    }
  }

  if (rotated_pixels != nullptr && rotated_pixels != resized_pixels) {
    free(rotated_pixels);
    rotated_pixels = nullptr;
  }
  if (resized_pixels != nullptr && resized_pixels != pixels) {
    free(resized_pixels);
    resized_pixels = nullptr;
  }
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

  preprocess(pixels, array, height, width, channel, config);

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
