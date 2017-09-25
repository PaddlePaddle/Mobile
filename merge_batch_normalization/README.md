# Merge Batch normarlization to Fc or conv layer based on Paddle


When a model training is finished, the parameters of batch_norm layer are fixed during inference. So, we can merget it into the convolution or fully connected layer.


For MORE details about batch normalization，see [here](https://arxiv.org/abs/1502.03167)

## Demo
这个例子将mobilenet转换成不带batch norm的mobilenet
我们用`mobilenet_with_bn`指代带batch norm的模型, `mobilenet_without_bn`指代不带batch norm的模型。


1. 首先我们通过`mobilenet_without_bn`来生模型参数，存储到`target_model.tar.gz`

	`$ python  pre_generate_model.py
	`

2. 将`mobilenet_with_bn`的模型参数`mobilenet_flowers102.tar.gz` 来进行融合batch_norm, 将融合的参数设置到`target_model.tar.gz` 中           

	`$ python merge_bn.py`
3. 验证模型融合正确性     

	`$ python verify`



### Note!
1. 在进行融合之前，得提前准备不带batch norm的模型配置，即 `./mobilenet/mobilenet_without_bn.py`, 配置不带batch norm的模型要注意三点， 第一，新的模型没有batch norm 层，第二，每个带batch norm的参数层（fc 或conv）， 要将`bias_attr`设置为True， 第三，每个带batch norm的参数层（fc 或 conv）将act属性(默认为`paddle.activation.Linear()`)设置为同batch norm相同的act(激励函数)，一般为`paddle.activation.Relu()`

2. 在mobilenet 和resnet上是ok的，其他的还没进行测试
