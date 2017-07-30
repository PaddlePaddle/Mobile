# Resnet18 pruning on flower102 
The pruning model can be download from here ([trained model](https://pan.baidu.com/s/1c1Hqxoc)). The accuracy is 97.35% on flower102, model size is 28M.



### Reproduce 
1. download the resnet18 model trained on flowers102.

	the `resnet18_flowers102.tar.gz`( [Download from BaiduCloud](https://pan.baidu.com/s/1pLlwg9X)) is the trained model on flower102. Accuracy is 97.353%

2. fine-tune the model   
`python train.py`


## Note!
the more details about `dynamic_pruning` can be found [here](https://github.com/PaddlePaddle/Paddle/pull/2603)

the `dynamic_prunig` has not been merged in paddle, if u want to use it, use the following paddle version:    

`$ git clone https://github.com/NHZlX/Paddle.git `     
`$ git checkout auto_pruning`     
`$ git remote add upstream https://github.com/PaddlePaddle/Paddle` 
`$ git fetch upstream`     
`$ git pull upstream develop`
