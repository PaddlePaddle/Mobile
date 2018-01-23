# PDCamera Android Demo

- [Intro](#Intro)
  - [Models](#Models)
  - [Demo](#Demo)
- [Quick Install](#Quick Install)
- [Development](#Development)
  - [Prepare Models](#Prepare Models)
  - [Integrate Paddle Library to Android](#Integrate Paddle Library to Android)

## Intro

This Android demo shows PaddlePaddle running SSD(Single Shot MultiBox Detector）Object detection on Android devices locally and offline. It loads a pretrained model with PaddlePaddle and uses camera to capture images and call PaddlePaddle's inference C API from library to show detected objects to users.

You can look at SSD model architecture, and how to train SSD Model with PaddlePaddle [here](https://github.com/PaddlePaddle/models/tree/develop/ssd) and a linux demo [here](https://github.com/PaddlePaddle/Mobile/tree/develop/Demo/linux)

### Models

We have provided two pretrained models for demo purposes. `pascal_mobilenet_300_66` can classify and detect 20 common categories while `face_mobilenet_160_91` is for human facial detection. Following table shows the corresponding accuracy for each model:

| Model                    | Dimensions | Accuracy |  Size |
| ------------------------ |:----------:| --------:|------:|
| [pascal\_mobilenet\_300\_66.paddle](http://cloud.dlnel.org/filepub/?uuid=39c325d9-b468-4940-ba47-d50c8ec5fd5b) | 300 x 300 | 66% | 23.2MB |
| [face\_mobilenet\_160\_91.paddle](http://cloud.dlnel.org/filepub/?uuid=038c1dbf-08b3-42a9-b2dc-efccd63859fb) | 160 x 160 | 91% | 18.4MB |

Click on the model name in the table to download the corresponding model through the browser.


### Demo

The app requires Camera and Storage permission. After granting permissions, tap on the screen will toggle settings as following:

- Models: Select Pascal MobileNet 300 or Face MobileNet 160 (Requires restart)
- Camera: Select Front or Back Camera
- Accuracy Threshold: Adjust threshold to filter more/less objects based on probability


For Pascal model, detected object will be highlighted as a bounding box with a classified object label and probability.
For Face model, detected face will be highlighted as a bounding box and probability.



## Quick Install

To simply run the demo with Android devices, scan the QR code below to download and install the apk. It runs on Android 5.0+ devices, support both arm64 and armv7a architecture.

<p align="center">
<img src="assets/qr_code_android.png" width = "30%" />
</p>


## Development

Use latest Android Studio for development. This demo requires a camera for object detection, therefore you must use an Android device for development and testing. Emulators will not work as they cannot access camera.

For developers, feel free to use this as a reference to start a new project. This demo fully demonstrates how to integrate Paddle C Library to Android, including CMake scripts and JNI wrapper and called from Java. Download Android NDK tools from Android Studio in order to debug c++ code. Click [here](https://developer.android.com/ndk/guides/index.html) for Android NDK guide.


### Prepare Models

Our models are too large to upload to Github. Create a model folder and add to assets folder. Download [face_mobilenet_160_91.paddle](http://cloud.dlnel.org/filepub/?uuid=038c1dbf-08b3-42a9-b2dc-efccd63859fb) and [pascal_mobilenet_300_66.paddle](http://cloud.dlnel.org/filepub/?uuid=39c325d9-b468-4940-ba47-d50c8ec5fd5b) to model folder.

Here, we are using **merged model**. If you want to know how to use model config file (such as `config.py`) and model params file from training(such as `params_pass_0.tar.gz`) to get **merged model** file, please see [Merged model config and parameters](../../../deployment/model/merge_config_parameters/README.md)


### Integrate Paddle Library to Android

- Follow this guide [Build PaddlePaddle for Android](https://github.com/PaddlePaddle/Paddle/blob/develop/doc/mobile/cross_compiling_for_android_en.md) to generate paddle libs(include, lib, third_party).
- Create a folder paddle-android and add to PROJECT_ROOT/AICamera/app . Put the 3 paddle libs folder(`include`,`lib`,`third_party`) under paddle-android.
- Add [FindPaddle.cmake](https://github.com/PaddlePaddle/Mobile/blob/develop/Demo/Android/AICamera/app/FindPaddle.cmake) in PROJECT_ROOT/AICamera/app to locate the dependencies of Paddle libraries. You should be using this file for your android project.
- Add [CMakeList.txt](https://github.com/PaddlePaddle/Mobile/blob/develop/Demo/Android/AICamera/app/CMakeList.txt) in PROJECT_ROOT/AICamera/app to link your C++ files to Paddle Libraries.
- Create JNI wrapper such as [image_recognizer_jni.cpp](https://github.com/PaddlePaddle/Mobile/blob/develop/Demo/Android/AICamera/app/src/main/cpp/image_recognizer_jni.cpp) to access C++ API in Paddle Library and a bridge calling from Java client.

