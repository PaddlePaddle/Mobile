# PDCamera Android Demo with SSD Model

This Android demo shows PaddlePaddle running SSD(Single Shot MultiBox Detector）Object detection on iOS devices locally and offline. It loads a pretrained model with PaddlePaddle and uses camera to capture images and call PaddlePaddle's inference ability to show detected objects to users.

You can look at SSD model architecture [here](https://github.com/PaddlePaddle/models/tree/develop/ssd) and a linux demo [here](https://github.com/PaddlePaddle/Mobile/tree/develop/Demo/linux)


## Download and run the app

To simply run the demo with Android devices, scan the QR code below to download and install the apk. It runs on Android 5.0+ devices, support both arm64 and armv7a architecture.


### QR code link

<img src="assets/qr_code_ios.png" width = "20%" />

### Demo screenshot

<img src="assets/demo_screenshot.jpg" width = "30%" />

Detected object will be highlighted as a bounding box with a classified object label and probability.


## Classifications
`pascal_mobilenet_300_66` models can only classify following 20 objects:

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

`face_mobilenet_160_91` can only classify human's face


## Settings

Simply tap on the screen to toggle settings

- Models: Select Pascal MobileNet 300 or Face MobileNet 160
- Camera: Select Front or Back Camera
- Accuracy Threshold: Adjust threshold to filter more/less objects based on probability


## Development or modify

Use latest Android Studio for development. This demo requires a camera for object detection, therefore you must use an Android device for development and testing. Emulators will not work as they cannot access camera.

For developers, feel free to use this as a reference to start a new project. This demo fully demonstrates how to integrate Paddle C Library to Android, including CMake scripts and JNI wrapper and called from Java. Download Android NDK tools from Android Studio in order to debug c++ code. Click [here](https://developer.android.com/ndk/guides/index.html) for Android NDK guide.


## Integrate Paddle C Library to Android

-Follow this guide [Build PaddlePaddle for Android](https://github.com/PaddlePaddle/Paddle/blob/develop/doc/mobile/cross_compiling_for_android_en.md) to generate paddle libs(include, lib, third_party).
-Create a folder paddle-android and add to PROJECT_ROOT/AICamera/app . Put the 3 paddle libs folder under paddle-android.
-Add [FindPaddle.cmake](https://github.com/PaddlePaddle/Mobile/blob/develop/Demo/Android/AICamera/app/FindPaddle.cmake) in PROJECT_ROOT/AICamera/app to locate the dependencies of Paddle libraries. You should be using this file for your android project.
-Add [CMakeList.txt](https://github.com/PaddlePaddle/Mobile/blob/develop/Demo/Android/AICamera/app/CMakeList.txt) in PROJECT_ROOT/AICamera/app to link your C++ files to Paddle Libraries.
-Create JNI wrapper such as [image_recognizer_jni.cpp](https://github.com/PaddlePaddle/Mobile/blob/develop/Demo/Android/AICamera/app/src/main/cpp/image_recognizer_jni.cpp) to access C++ API in Paddle Library and a bridge calling from Java client.


## Download Models

Our models are too large to upload to Github. Create a model folder and add to assets folder. Download [face_mobilenet_160_91.paddle](http://cloud.dlnel.org/filepub/?uuid=038c1dbf-08b3-42a9-b2dc-efccd63859fb) and [pascal_mobilenet_300_66.paddle](http://cloud.dlnel.org/filepub/?uuid=39c325d9-b468-4940-ba47-d50c8ec5fd5b) to the model folder.


## Accuracy

| Model                    | Dimensions | Accuracy |
| ------------------------ |:----------:| --------:|
| face_mobilenet_160_91    | 160x160    | 91%      |
| pascal_mobilenet_300_66  | 300x300    | 66%      |
