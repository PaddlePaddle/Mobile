# MobileNet and Resnet18 test on oxford flowers 102 dataset

### we did four tests on mobilenet and resnet18，they are:

 - train the mobilenet model on flowers102 using pretrained imagenet model.  
 - train the resnet18 model on flower102 using pretrained imagenet model.
 - fine-tune the mobilenet model trained on flower102 using dynamic pruning.
 - fine-tune the resnet18 model trained on flower102 using dynamic pruning. 


### result
we evaluated the model in accuracy and modle size.

|| mobilenet | resnet18|mobilenet pruning| resnet18 pruning|
|---| ------------- |:-------------:|:--:|:--:|
|accuracy|  0.9716    | 0.97353 |0.970 |0.9735|
|model size| 12M | 40M |  4.3M |28M|




You can find more deatils of these models in    
`./mobilenet`   
`./resnet18`    
`./mobilenet_pruning`  
`./resnet18_pruning`
