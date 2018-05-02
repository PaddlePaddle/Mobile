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

#include "image_converter.h"

static inline uint8_t* YUV2RGB(int nY, int nU, int nV) {
  int kMaxChannelValue = 262143;

  nY -= 16;
  nU -= 128;
  nV -= 128;
  if (nY < 0) nY = 0;

  int nR = 1192 * nY + 1634 * nV;
  int nG = 1192 * nY - 833 * nV - 400 * nU;
  int nB = 1192 * nY + 2066 * nU;

  nR = nR > 0 ? nR : 0;
  nG = nG > 0 ? nG : 0;
  nB = nB > 0 ? nB : 0;

  nR = nR < kMaxChannelValue ? nR : kMaxChannelValue;
  nG = nG < kMaxChannelValue ? nG : kMaxChannelValue;
  nB = nB < kMaxChannelValue ? nB : kMaxChannelValue;

  uint8_t* result = new uint8_t[3];
  result[0] = nR / 1000;
  result[1] = nG / 1000;
  result[2] = nB / 1000;

  return result;
}

void ConvertYUV420ToARGB8888(const uint8_t* const yData,
                             const uint8_t* const uData,
                             const uint8_t* const vData,
                             uint8_t* const output,
                             const int width,
                             const int height,
                             const int y_row_stride,
                             const int uv_row_stride,
                             const int uv_pixel_stride) {
  uint8_t* out = output;

  for (int y = 0; y < height; y++) {
    const uint8_t* pY = yData + y_row_stride * y;

    const int uv_row_start = uv_row_stride * (y >> 1);
    const uint8_t* pU = uData + uv_row_start;
    const uint8_t* pV = vData + uv_row_start;

    for (int x = 0; x < width; x++) {
      const int uv_offset = (x >> 1) * uv_pixel_stride;
      uint8_t* result = YUV2RGB(pY[x], pU[uv_offset], pV[uv_offset]);
      for (int i = 0; i < 3; i++) {
        *out++ = result[i];
      }
    }
  }
}
