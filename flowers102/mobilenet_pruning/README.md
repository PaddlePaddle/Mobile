## Mobilenet pruning on flower102 
The pruning model can be download from here ([trained model](https://pan.baidu.com/s/1ge8wOp1)). The accuracy is 97.0%, model size is 4.3M.


### Reproduce 
1. download the mobilenet model trained on flower102.

	the `mobilenet_flowers102.tar.gz`( [Download from BaiduCloud](https://pan.baidu.com/s/1slK3wlB)) is the trained model on flower102. Accuracy is 97.16%.

2. train the model   
`python train.py`


## Note!
more details about `dynamic_pruning` can be found [here](https://github.com/PaddlePaddle/Paddle/pull/2603)

the `dynamic_prunig` has not been merged in paddle, if u want to use it, use the following paddle version:    

`$ git clone https://github.com/NHZlX/Paddle.git `     
`$ git checkout auto_pruning`     
`$ git remote add upstream https://github.com/PaddlePaddle/Paddle` 
`$ git fetch upstream`     
`$ git pull upstream develop`
