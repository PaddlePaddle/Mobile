# Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserved
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License

import gzip

import paddle.v2 as paddle
from mobilenet.mobilenet_without_bn import mobile_net

def generate_model(net, model_path):
    with gzip.open(model_path, 'w') as f:
        paddle.parameters.create(net).to_tar(f)
    print 'generate ',model_path,' SUCCESS!'

if __name__ == '__main__':
        img_size = 3 * 224 * 224
        class_num = 102
	net = mobile_net(img_size, class_num)
        model_path = './mobilenet/target_model.tar.gz' 
        generate_model(net, model_path)
