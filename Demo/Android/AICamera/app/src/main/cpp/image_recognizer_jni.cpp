/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserve.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License */

#include <jni.h>
#include <android/asset_manager_jni.h>
#include <string>
#include <memory>
#include "paddle_image_recognizer.h"

extern "C" {

static std::shared_ptr<ImageRecognizer> recognizer;

JNIEXPORT void
Java_com_paddlepaddle_aicamera_ImageRecognizer_init(JNIEnv *env,
                                          jobject thiz,
                                          jobject jasset_manager,
                                          jstring jmodel_path,
                                          jint jheight,
                                          jint jwidth,
                                          jint jchannel,
                                          jfloatArray jmeans) {


    AAssetManager *aasset_manager = AAssetManager_fromJava(env, jasset_manager);


    recognizer = std::make_shared<ImageRecognizer>();

    long size = 0;
    const char *model_path = env->GetStringUTFChars(jmodel_path, 0);
//    void *merged_model = read_binary(aasset_manager, model_path, &size);

    const float *m = env->GetFloatArrayElements(jmeans, 0);
    std::vector<float> means(m, m + sizeof m / sizeof m[0]);
    ImageRecognizer recognizer1;
    recognizer1.init(model_path, jheight, jwidth, jchannel, means);

    env->ReleaseStringUTFChars(jmodel_path, model_path);

}

JNIEXPORT jfloatArray
Java_com_paddlepaddle_aicamera_ImageRecognizer_infer(JNIEnv *env,
                                                     jobject thiz,
                                                     jbyteArray jpixels,
                                                     jint jheight,
                                                     jint jwidth,
                                                     jint jchannel) {

    ImageRecognizer::Result result;
    image::Config config(image::kBGR, image::CLOCKWISE_R90);

    int len = env->GetArrayLength (jpixels);
    unsigned char* pixels = new unsigned char[len];
    env->GetByteArrayRegion (jpixels, 0, len, reinterpret_cast<jbyte*>(pixels));

    recognizer->infer(pixels, jheight, jwidth, jchannel, config, result);

}

JNIEXPORT void
Java_com_paddlepaddle_aicamera_ImageRecognizer_release(JNIEnv *env,
                                                     jobject thiz) {
    recognizer->release();
}

}

