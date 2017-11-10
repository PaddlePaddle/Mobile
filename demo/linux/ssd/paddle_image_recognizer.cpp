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
#include <algorithm>
#include <cmath>

namespace image {

#if 0
void resize_hwc(const unsigned char* raw_data, unsigned char* resized_data, const size_t height, const size_t width, const size_t channel, const size_t resized_height, const size_t resized_width) {
    int iWidthSrc = width;
    int iHeightSrc = height;
    int stepDst = resized_width * channel;
    int stepSrc = width * channel;
    float scale_x = static_cast<float>(width) / resized_width;
    float scale_y = static_cast<float>(height) / resized_height;

    for (int j = 0; j < resized_height; j++) {
        float fy = (float)((j + 0.5) * scale_y - 0.5);
        int sy = std::floor(fy);
        fy -= sy;
        sy = std::min(sy, iHeightSrc - 2);
        sy = std::max(0, sy);

        short cbufy[2];
        if ((1.f - fy) * 2048 < 0) {
          cbufy[0] = 0;
        } else if ((1.f - fy) * 2048 > 32767) {
          cbufy[0] = 32767;
        } else {
          cbufy[0] = short((1.f - fy) * 2048);
        }
        cbufy[1] = 2048 - cbufy[0];

        for (int i = 0; i < resized_width; ++i) {
            float fx = (float)((i + 0.5) * scale_x - 0.5);
            int sx = floor(fx);
            fx -= sx;

            if (sx < 0) {
                fx = 0, sx = 0;
            }
            if (sx >= iWidthSrc - 1) {
                fx = 0, sx = iWidthSrc - 2;
            }

            short cbufx[2];
            if ((1.f - fx) * 2048 < 0) {
                cbufx[0] = 0;
            } else if ((1.f - fx) * 2048 > 32767) {
                cbufx[0] = 32767;
            } else {
              cbufx[0] = short((1.f - fx) * 2048);
            }
            cbufx[1] = 2048 - cbufx[0];
            for (int k = 0; k < channel; ++k) {
                resized_data[j * stepDst + 3 * i + k] =
                        (unsigned int)((char(raw_data[sy*stepSrc + 3*sx + k]) * cbufx[0] * cbufy[0] +
                        char(raw_data[(sy+1)*stepSrc + 3*sx + k]) * cbufx[0] * cbufy[1] +
                        char(raw_data[sy*stepSrc + 3*(sx+1) + k]) * cbufx[1] * cbufy[0] +
                        char(raw_data[(sy+1)*stepSrc + 3*(sx+1) + k]) * cbufx[1] * cbufy[1]) >> 22);
            }
        }
    }
}
#else
void resize_hwc(const unsigned char* inputImage,
                unsigned char* outputImage,
                const size_t srcHeight,
                const size_t srcWidth,
                const size_t nChannels,
                const size_t dstHeight,
                const size_t dstWidth) {
  float ratioX = srcWidth * 1.0 / dstWidth;
  float ratioY = srcHeight * 1.0 / dstHeight;

  for (int i = 0; i < dstHeight; i++) {
    float newY = i * ratioY;

    int y1 = ((int)newY > (srcHeight - 1)) ? (srcHeight - 1) : (int)newY;
    int y2 = y1 + 1;

    float b1 = y2 - newY;
    float b2 = newY - y1;

    for (int j = 0; j < dstWidth; j++) {
      float newX = j * ratioX;
      int x1 = ((int)newX > (srcWidth - 1)) ? (srcWidth - 1) : (int)newX;
      int x2 = x1 + 1;

      float a1 = x2 - newX;
      float a2 = newX - x1;

      unsigned char* ptDst = outputImage + (i * dstWidth + j) * nChannels;
      unsigned char* ptSrc = const_cast<unsigned char*>(inputImage) +
                             (y1 * srcWidth + x1) * nChannels;
      int p1 = srcWidth * nChannels;
      int p2 = p1 + nChannels;

      if (x1 == srcWidth - 1 && y1 == srcHeight - 1) {
        memcpy(ptDst, ptSrc, nChannels * sizeof(unsigned char));
      } else if (x1 == srcWidth - 1) {
        for (int k = 0; k < nChannels; k++) {
          *ptDst = (unsigned char)((*(ptSrc)) * b1 + (*(ptSrc + p1)) * b2);
          ptDst++;
          ptSrc++;
        }
      } else if (y1 == srcHeight - 1) {
        for (int k = 0; k < nChannels; k++) {
          *ptDst = (unsigned char)((*ptSrc) * a1 + (*(ptSrc + nChannels)) * a2);
          ptDst++;
          ptSrc++;
        }
      } else {
        // x1=srcWidth-1或者y1=srcHeight-1，则下述公式中指针访问会越界，因此将三种情况分拆出来各自处理
        for (int k = 0; k < nChannels; k++) {
#if 0
                    *ptDst = (unsigned char) (
                            ((*ptSrc       ) * a1 + (*(ptSrc + nChannels)) * a2) * b1
                            + ((*(ptSrc + p1)) * a1 + (*(ptSrc + p2       )) * a2) * b2);
#else
          float pixel_00 = (float)ptSrc[0 + k];
          float pixel_01 = (float)ptSrc[nChannels + k];
          float pixel_10 = (float)ptSrc[p1 + k];
          float pixel_11 = (float)ptSrc[p2 + k];

          ptDst[k] = (unsigned char)((pixel_00 * a1 + pixel_01 * a2) * b1 +
                                     (pixel_10 * a1 + pixel_11 * a2) * b2);
#endif
        }
      }
    }  // for j
  }    // for i
}
#endif

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
                                 const size_t channel) {
// if (height == normed_height && width == normed_width && channel ==
// normed_channel) {
// } else {
// }

#if 0
  size_t resized_height = 0;
  size_t resized_width = 0;
  if (height > width) {
    resized_width = normed_width;
    resized_height = normed_height * height / width;
  } else /* height <= width */ {
    resized_width = normed_width * width / height;
    resized_height = normed_height;
  }
#else
  size_t resized_height = normed_height_;
  size_t resized_width = normed_width_;
#endif

  // Bilinear Interpolation Resize
  unsigned char* resized_pixels = (unsigned char*)malloc(
      resized_height * resized_width * channel * sizeof(unsigned char));
  image::resize_hwc(pixels,
                    resized_pixels,
                    height,
                    width,
                    channel,
                    resized_height,
                    resized_width);
  // ImageWriter()("images/resized.jpg", resized_pixels, resized_height,
  // resized_width, channel, kHWC);

  if (resized_height == normed_height_ && resized_width == normed_width_) {
    // HWC -> CHW, RGBA -> RGB
    size_t index = 0;
    for (size_t c = 0; c < normed_channel_; ++c) {
      for (size_t h = 0; h < normed_height_; ++h) {
        for (size_t w = 0; w < normed_width_; ++w) {
          normed_pixels[index] =
              static_cast<float>(
                  resized_pixels[(h * resized_width + w) * channel + c]) -
              means_[c];
          index++;
        }
      }
    }
  }

  free(resized_pixels);
  resized_pixels = nullptr;
}

void ImageRecognizer::infer(const unsigned char* pixels,
                            const size_t height,
                            const size_t width,
                            const size_t channel,
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

  preprocess(pixels, array, height, width, channel);

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
