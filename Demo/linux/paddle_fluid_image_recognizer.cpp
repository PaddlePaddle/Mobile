/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserve.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License */

#include "paddle_fluid_image_recognizer.h"
#include <string.h>

void ImageRecognizer::init(const std::string& model_path,
                           const std::string& params_path,
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

  // 1. Define place, executor, scope
  place_ = new paddle::platform::CPUPlace();
  executor_ = new paddle::framework::Executor(*place_);
  scope_ = new paddle::framework::Scope();

  // 2. Initialize the inference_program and load parameters
  inference_program_ =
      paddle::inference::Load(*executor_, *scope_, model_path, params_path);
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

  // 3. Get the feed_target_names and fetch_target_names
  const std::vector<std::string>& feed_target_names =
      inference_program_->GetFeedTargetNames();
  const std::vector<std::string>& fetch_target_names =
      inference_program_->GetFetchTargetNames();

  if ((feed_target_names.size() != 1) || (fetch_target_names.size() != 1)) {
    return;
  }

  // 4. Prepare inputs: set up maps for feed targets
  std::map<std::string, const paddle::framework::LoDTensor*> feed_targets;
  paddle::framework::LoDTensor input;
  float* input_ptr =
      input.mutable_data<float>({/* N */ static_cast<int64_t>(1),
                                 /* C */ static_cast<int64_t>(normed_channel_),
                                 /* H */ static_cast<int64_t>(normed_height_),
                                 /* W */ static_cast<int64_t>(normed_width_)},
                                paddle::platform::CPUPlace());
  feed_targets[feed_target_names[0]] = &input;

  image::utils::normalize(pixels,
                          input_ptr,
                          height,
                          width,
                          channel,
                          normed_height_,
                          normed_width_,
                          normed_channel_,
                          means_,
                          config);

  // 5. Define Tensor to get the outputs: set up maps for fetch targets
  std::map<std::string, paddle::framework::LoDTensor*> fetch_targets;
  paddle::framework::LoDTensor output;
  fetch_targets[fetch_target_names[0]] = &output;

  // 6. Run the inference program
  executor_->Run(*inference_program_, scope_, feed_targets, fetch_targets);

  // 7. Get the result
  result.data = output.data<float>();
  result.height = output.dims()[0];
  result.width = output.dims()[1];
}

void ImageRecognizer::release() {
  delete place_;
  delete executor_;
  delete scope_;
}
