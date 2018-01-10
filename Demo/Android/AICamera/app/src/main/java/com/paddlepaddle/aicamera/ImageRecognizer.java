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
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

public class ImageRecognizer {

    static {
        System.loadLibrary("paddle_image_recognizer");
    }
    private static final String TAG = "ImageRecognizer";
    private static float[] means = {104, 117, 124};

    private static final String[] LABELS = {
            "background" , "aeroplane", "bicycle"  , "background" ,
            "boat"       , "bottle"   , "bus"      , "car"        ,
            "cat"        , "chair"    , "cow"      , "diningtable",
            "dog"        , "horse"    , "motorbike", "person"     ,
            "pottedplant", "sheep"    , "sofa"     , "train"      ,
            "tvmonitor" };

    private long mImageRecognizer = 0;

    public ImageRecognizer(Context context, SSDModel model) {
        String modelPath = "models/" + model.modelFileName;
        mImageRecognizer = init(context.getAssets(), modelPath, model.height, model.width, 3, means);
    }

    public List<SSDData> infer(byte[] pixels, int height, int width, int channel, float filterScore) {

        float[] result = infer(mImageRecognizer, pixels, height, width, channel);

        int w = 7;
        int h = result.length / w;

        List<SSDData> resultList = new ArrayList<SSDData>();

        Log.d("ZZZ", "h = " + h);

        for (int i = 0; i < h; i++) {
            float score = result[i * w + 2];

            Log.d("ZZZ", "score = " + score);

            if (score < filterScore) continue;
            SSDData ssdData = new SSDData();
            ssdData.label = LABELS[(int) result[i * w + 1]];
            ssdData.accuracy = score;


            ssdData.xmin = result[i * w + 3];
            ssdData.ymin = result[i * w + 4];
            ssdData.xmax = result[i * w + 5];
            ssdData.ymax = result[i * w + 6];

            resultList.add(ssdData);
        }

        return resultList;
    }

    public void destroy() {
        release();
    }

    private native long init(AssetManager assetManager, String modelPath, int normHeight, int normWidth, int normChannel, float[] means);

    private native float[] infer(long imageRecognizer,
                                 byte[] pixels,
                                 int height,
                                 int width,
                                 int channel);

    private native void release();
}
