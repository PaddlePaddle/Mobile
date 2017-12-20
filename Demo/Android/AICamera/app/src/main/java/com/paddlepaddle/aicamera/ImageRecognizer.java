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
package com.paddlepaddle.aicamera;

import android.content.Context;
import android.content.res.AssetManager;

import java.util.ArrayList;
import java.util.List;

public class ImageRecognizer {

    static {
        System.loadLibrary("paddle_image_recognizer");
    }
    private static final String TAG = "ImageRecognizer";
    private static float[] means = {104, 117, 124};

    private long mImageRecognizerPtr = 0;

    public ImageRecognizer(Context context) {
        //copy model to file
        CopyModelUtils.copyFilesToStorage(context);

        String modelPath = "pascal_mobilenet_300_66.paddle";
        mImageRecognizerPtr = init(context.getAssets(), context.getExternalCacheDir().getAbsolutePath() + "/" + modelPath, 300, 300, 3, means);
    }

    public List<SSDData> infer(byte[] pixels, int height, int width, float filterScore) {

        float[] result = infer(mImageRecognizerPtr, pixels, height, width, 4);

        List<SSDData> resultList = new ArrayList<SSDData>();
        SSDData data = new SSDData();
        data.accuracy = 0;
        resultList.add(data);
        return resultList;
    }

    public void destroy() {
        release();
    }


    private native long init(AssetManager assetManager, String modelPath, int normHeight, int normWidth, int normChannel, float[] means);

    private native float[] infer(long imageRecognizerPtr,
                                     byte[] pixels,
                                     int height,
                                     int width,
                                     int channel);

    private native void release();

}



