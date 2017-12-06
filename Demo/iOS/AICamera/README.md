# PDCamera iOS Demo with SSD Model

This iOS demo shows PaddlePaddle running SSD(Single Shot MultiBox Detector）Object detection on iOS devices locally and offline. It loads a pretrained model with PaddlePaddle and uses camera to capture images and call PaddlePaddle's inference ability to show detected objects to users.

You can look at SSD model architecture [here](https://github.com/PaddlePaddle/models/tree/develop/ssd) and a linux demo [here](https://github.com/PaddlePaddle/Mobile/tree/develop/Demo/linux)


## Download and run the app

To simply run the demo with iPhone/iPad, scan the QR code below, click "Install PDCamera" in the link and the app will be downloaded in the background.
After installed, go to Settings -> General -> Device Management -> Baidu USA llc -> Trust "Baidu USA llc"


### QR code link

[image](https://github.com/PaddlePaddle/Mobile/tree/develop/Demo/iOS/AICamera/assets/qr_code_ios.png)

### Demo screenshot

[image](https://github.com/PaddlePaddle/Mobile/tree/develop/Demo/iOS/AICamera/assets/demo_screenshot.jpg)

Detected object will be highlighted as a bounding box with a classified object label and probability.


## Classifications
pascal_mobilenet_300_66 and vgg_ssd_net models can only classify following 20 objects:

- aeroplane
- bicycle
- background
- boat
- bottle
- bus
- car
- cat
- chair
- cow
- diningtable
- dog
- horse
- motorbike
- person
- pottedplant
- sheep
- sofa
- train
- tvmonitor

face_mobilenet_160_91 can only classify human's face


## Settings

Simply tap on the screen to toggle settings

- Models: Select Pascal MobileNet 300 or Face MobileNet 160, App will exit, need to launch to restart.
- Camera: Toggle Front/Back Camera. App will exit, need to launch to restart. 
- Accuracy Threshold: Adjust threshold to filter more/less objects based on probability
- Time Refresh Rate: Adjust the time to refresh bounding box more/less frequently


## Development or modify

Use latest XCode for development. This demo requires a camera for object detection, therefore you must use a device (iPhone or iPad) for development and testing. Simulators will not work as they cannot access camera.

For developers, feel free to use this as a reference to start a new project. This demo fully demonstrates how to integrate Paddle C Library to iOS and called from Swift.

Swift cannot directly call C API, in order to have client in Swift work, create Objective-C briding header and a Objective-C++ wrapper (.mm files) to access paddle APIs.


## Integrate Paddle C Library to iOS

-Follow this guide [Build PaddlePaddle for iOS](https://github.com/PaddlePaddle/Paddle/blob/develop/doc/mobile/cross_compiling_for_ios_cn.md) to generate paddle libs(include, lib, third_party).
-Create a folder paddle-ios and add to project root. Put the 3 paddle libs folder under paddle-ios.
- Add the `include` directory to **Header Search Paths**
![image](https://user-images.githubusercontent.com/12538138/32491809-b215cf7a-c37d-11e7-87f8-3d45f07bc63e.png)

- Add the `Accelerate.framework` or `veclib.framework` to your project, if your PaddlePaddle is built with `IOS_USE_VECLIB_FOR_BLAS=ON`
- Add the libraries of paddle, `libpaddle_capi_layers.a` and `libpaddle_capi_engine.a`, and all the third party libraries to your project
    <img src="https://user-images.githubusercontent.com/12538138/32492222-2ecef414-c37f-11e7-9913-b90fc88be10f.png" width = "30%" />

- Set `-force_load` for `libpaddle_capi_layers.a`
![image](https://user-images.githubusercontent.com/12538138/32492328-8504ebae-c37f-11e7-98b5-41615519fbb3.png)


## Download Models

Our models are too large to upload to Github. Create a model folder and add to project root. Download [face_mobilenet_160_91.paddle](http://cloud.dlnel.org/filepub/?uuid=038c1dbf-08b3-42a9-b2dc-efccd63859fb) and [pascal_mobilenet_300_66.paddle](http://cloud.dlnel.org/filepub/?uuid=39c325d9-b468-4940-ba47-d50c8ec5fd5b) to the model folder.

(Optional) VGG model is relatively large and takes much higher memory(~800Mb), power, and much slower (~1.5secs) on each inference but it has slightly accuracy gain (See below section)
Note: Only runs on iPhone6s or above (iPhone 6 or below will crash due to memory limit)
If you want to try it out, download [vgg_ssd_net.paddle](http://cloud.dlnel.org/filepub/?uuid=1116a5f3-7762-44b5-82bb-9954159cb5d4), then go to
XCode target -> Bulid Phases -> Copy Bundle Resources, click '+' to add vgg_ssd_net.paddle


## Accuracy

| Model                    | Dimensions | Accuracy |
| ------------------------ |:----------:| --------:|
| face_mobilenet_160_91    | 160x160    | 91%      |
| pascal_mobilenet_300_66  | 300x300    | 66%      |
| vgg_ssd_net              | 300x300    | 71%      |

