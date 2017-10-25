
# Merge model config and parameters

Integrate the model configuration and model parameters into one file. 

## Demo

### Step 1: Prepartions

**Model Config :** [Mobilenet model config](../../models/mobilenet.py).    
**Model Parameters:** [Mobilenet model param pretrained on flower102 download](https://pan.baidu.com/s/1geHkrw3) 

### Step2: Merge 

1. modify the configuration in config.py
	- NET_OUT : `the model config output`
	- MODEL_PARAM : `the path of model parameters`
	- MERGED_FILE : `the output path of merged file`



2. merge the config and parameters    
``` 
$ python merge_config_and_params.py
```


