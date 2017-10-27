
# Merge model config and parameters

Integrate the model configuration and model parameters into one file. 
The merged file is used in our capi forward predict program.

## Demo

This applies to all PaddlePaddle v2 models, we show a demo of mobilenet.

### Step 1: Prepartions

**Model Config :** [Mobilenet model config](../../models/mobilenet.py).    
**Model Parameters:** [Mobilenet model param pretrained on flower102 download](https://pan.baidu.com/s/1geHkrw3) 

### Step2: Merge 

Run the following code

```
from paddle.util.merge_model import merge_v2_model

# import your network configuration
from mobilenet import mobile_net
        
net = mobile_net(3*224*224, 102, 1.0)
param_file = './mobilenet_flowers102.tar.gz'
output_file = './output.paddle'
        
merge_v2_model(net, param_file, output_file)

```
