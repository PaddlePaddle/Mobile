package com.paddlepaddle.aicamera;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private ImageRecognizer mImageRecognizer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mImageRecognizer = new ImageRecognizer(this);

        int height = 300;
        int width = 300;
        int channel = 3;
        byte[] pixels = new byte[height * width * channel];
        for (int i = 0; i < height * width * channel; ++i) {
            pixels[i] = (byte) i;
        }

        mImageRecognizer.infer(pixels, height, width, channel, 0.6f);

    // Example of a call to a native method
    TextView tv = (TextView) findViewById(R.id.sample_text);
//    tv.setText(stringFromJNI());
    }


}
