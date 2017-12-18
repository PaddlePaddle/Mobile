# Merge Model Tools

## Contents
- [Introduction](introduction)
- [Using the Merge Model Tool](using-the-merge-model-tool)
- [Optimizing Tools for Deployment](optimizing-tools-for-deployment)
	- [Reduce the merged model size](reduce-the-merged-model-size)
	- [Merge batch norms](merge-batch-norms)

	
### Introduction

When we have finished training, and we want to use the C++ interface to deploy the model to the real application. We need to merge config files(.py) and parameter files(.tar.gz) into a single file.

This script is designed to take a model config(.py) and the parameters values stored in a gzip file, and output a merged model.

### Using the Merge Model Tool

We should prepare model config and model parameters first.   
We offer the download link to mobilenet [config](https://github.com/PaddlePaddle/Mobile/blob/develop/deployment/models/mobilenet.py) and [paramters](https://pan.baidu.com/s/1geHkrw3).

Merge Model can be done using the script below (and modifying the arguments According to your needs):

```
from paddle.utils.merge_model import merge_v2_model
# import your network configuration
from mobilenet import mobile_net

net = mobile_net(3 * 224 * 224, 102, 1.0)
merge_v2_model(net = net,
               param_file = './mobilenet_flowers102.tar.gz',
               output_file = './output.paddle')
```

The arguments here are specifying the net config, where to read the parameter file, where to write the merged model to.


### Optimizing Tools for Deployment
This section provides some commonly used optimizing tools, which is very useful for mobile deployment of the model.

#### Reduce the merged model size

If we want to deploy the model to mobile, the size of the model directly affects the size of the app. So we want our model to be as small as possible. 

IOS and Android application packages are compressed before downloading.
The principle of compression is that if there are a lot of repeated numbers in the compressed file, then the compression rate will be very high. But in our model parameters, there is very low repetition, so the compression is limited. 
By the way of rounding, the approximate values in model parameters will be converted to the same value, the parameters after round will have a lot of repetition, so the compression will greatly reduce the size of our models.

Rounding parameters can be done using the script below:

```
from paddle.utils.merge_model import merge_v2_model
# import your network configuration
from mobilenet import mobile_net

net = mobile_net(3 * 224 * 224, 102, 1.0)
merge_v2_model(net = net,
               param_file = './mobilenet_flowers102.tar.gz',
               output_file = './output.paddle'
               with_rounding = True)
```
The arguments here are specifying the net config, where to read the parameter file, where to write the merged model to, wheather to rounding parameters.

The result merged model is the same size as before, but if you run `gzip` on the model, it would be about 70% smaller than the original.

#### Merge batch norms 
Many of the current models add batch norm after conv or fc layers during training. During inference, we can integrate these two operations into one operation.
We give an example of `Conv + Batch norm`. 


1. Conv process   
<img width="194" alt="b850c3129cdd088e1f53b030abd3de63" src="https://user-images.githubusercontent.com/5595332/34099448-86e25496-e41a-11e7-9558-31b924a1f09f.png"> 
2. Batch norm process     
<img width="151" alt="a244a4af396f292b3eae899acada6746" src="https://user-images.githubusercontent.com/5595332/34099455-8a0914d4-e41a-11e7-8dbb-8cb42fd18d6a.png">  
<img width="139" alt="81ed60ae38f6583082d87e4f5c276bfe" src="https://user-images.githubusercontent.com/5595332/34099459-8c9818a8-e41a-11e7-9487-f801484485da.png">
3. Integrate into one process
<img width="425" alt="71bdc09682f451ead8bba5c83fdcdea0" src="https://user-images.githubusercontent.com/5595332/34099412-6c2d3080-e41a-11e7-9595-876ee8d75204.png">

Merge batch normalization can be done using the script below:


```
from paddle.utils.merge_model import merge_v2_model
# import your network configuration
from mobilenet import mobile_net

net = mobile_net(3 * 224 * 224, 102, 1.0)
merge_v2_model(net = net,
               param_file = './mobilenet_flowers102.tar.gz',
               output_file = './output.paddle'
               merge_batch_normazlization = True)
```
The arguments here are specifying the net config, where to read the parameter file, where to write the merged model to, wheather to merge batch normalization.

Merge batch normalization speeds up the Inference by around 30%.