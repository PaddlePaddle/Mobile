package com.paddlepaddle.aicamera;

import android.Manifest;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureFailure;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.util.Size;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Arrays;
import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnCheckedChanged;
import butterknife.OnClick;
import butterknife.Optional;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getSimpleName();

    private static final int PERMISSIONS_REQUEST = 1;

    private CameraCaptureSession mCaptureSession;
    private CameraDevice mCameraDevice;

    private HandlerThread mCaptureThread; //background thread for capturing image
    private Handler mCaptureHandler;
    private HandlerThread mInferThread; //background thread for inferencing from paddle
    private Handler mInferHandler;

    private ImageReader mImageReader;
    private ImageRecognizer mImageRecognizer;

    private Size mPreviewSize;
    private byte[] mRgbBytes;

    private boolean mInProcessing;

    private SSDModel mModel = SSDModel.PASCAL_MOBILENET_300;
    private boolean mBackCamera = true;
    private float mAccuracyThreshold = 0.5f; //only display above this threshold
    private SharedPreferences mPrefs;
    private boolean mSettingsShown;
    private SettingsViewHolder mSettingsViewHolder;

    @BindView(R.id.textureView)
    AutoFitTextureView mTextureView;

    @BindView(R.id.ssdLayerView)
    SSDLayerView mSSDLayerView;

    @BindView(R.id.settingsContainer)
    ViewGroup mSettingsContainer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ButterKnife.bind(this);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        loadSettings();
        mImageRecognizer = new ImageRecognizer(this, mModel);
    }

    private boolean hasPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            return checkSelfPermission(Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED &&
                    checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED;
        } else {
            return true;
        }
    }

    private void requestPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (shouldShowRequestPermissionRationale(Manifest.permission.CAMERA) ||
                    shouldShowRequestPermissionRationale(Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
                Toast.makeText(MainActivity.this,
                        "Camera AND storage permission are required for this demo", Toast.LENGTH_LONG).show();
            }
            requestPermissions(new String[]{Manifest.permission.CAMERA, Manifest.permission.WRITE_EXTERNAL_STORAGE}, PERMISSIONS_REQUEST);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == PERMISSIONS_REQUEST) {
            if (grantResults.length > 0
                    && grantResults[0] == PackageManager.PERMISSION_GRANTED
                    && grantResults[1] == PackageManager.PERMISSION_GRANTED) {
                startCapture();
            } else {
                requestPermission();
            }
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        startCaptureThread();
        startInferThread();

        if (mTextureView.isAvailable()) {
            startCapture();
        } else {
            mTextureView.setSurfaceTextureListener(new TextureView.SurfaceTextureListener() {
                @Override
                public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
                    startCapture();
                }

                @Override
                public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
                }

                @Override
                public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
                    return true;
                }

                @Override
                public void onSurfaceTextureUpdated(SurfaceTexture surface) {

                }
            });
        }
    }

    @Override
    protected void onPause() {
        closeCamera();
        stopCaptureThread();
        stopInferThread();
        super.onPause();
    }

    private void startCaptureThread() {
        mCaptureThread = new HandlerThread("capture");
        mCaptureThread.start();
        mCaptureHandler = new Handler(mCaptureThread.getLooper());
    }

    private void startInferThread() {
        mInferThread = new HandlerThread("inference");
        mInferThread.start();
        mInferHandler = new Handler(mInferThread.getLooper());
    }

    private void stopCaptureThread() {
        mCaptureThread.quitSafely();
        try {
            mCaptureThread.join();
            mCaptureThread = null;
            mCaptureHandler = null;
        } catch (final InterruptedException e) {
        }
    }

    private void stopInferThread() {
        mInferThread.quitSafely();
        try {
            mInferThread.join();
            mInferThread = null;
            mInferHandler = null;
        } catch (final InterruptedException e) {
        }
    }

    private void startCapture() {
        if (!hasPermission()) {
            requestPermission();
            return;
        }

        final CameraManager manager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);

        String cameraIdAvailable = null;
        try {
            for (final String cameraId : manager.getCameraIdList()) {
                final CameraCharacteristics characteristics = manager.getCameraCharacteristics(cameraId);

                final Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);
                if (facing != null && facing == (mBackCamera ? CameraCharacteristics.LENS_FACING_BACK : CameraCharacteristics.LENS_FACING_FRONT)) {
                    cameraIdAvailable = cameraId;
                    break;
                }
            }
        } catch (CameraAccessException e) {
            android.util.Log.e(TAG, "Start Capture exception", e);
        }

        try {
            final CameraCharacteristics characteristics = manager.getCameraCharacteristics(cameraIdAvailable);

            final StreamConfigurationMap map =
                    characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
            mPreviewSize = ImageUtils.chooseOptimalSize(map.getOutputSizes(SurfaceTexture.class),
                    mTextureView.getHeight(),
                    mTextureView.getWidth());
            mTextureView.setAspectRatio(
                    mPreviewSize.getHeight(), mPreviewSize.getWidth());
            float aspectRatio = mPreviewSize.getWidth() * 1.0f / mPreviewSize.getHeight();
            mSSDLayerView.setTextureViewDimen(mTextureView.getWidth(), (int) (mTextureView.getWidth() * aspectRatio));

            manager.openCamera(cameraIdAvailable, new CameraDevice.StateCallback() {
                @Override
                public void onOpened(@NonNull CameraDevice camera) {
                    mCameraDevice = camera;
                    createCameraPreviewSession();
                }

                @Override
                public void onDisconnected(@NonNull CameraDevice camera) {
                    camera.close();
                    mCameraDevice = null;
                }

                @Override
                public void onError(@NonNull CameraDevice camera, final int error) {
                    android.util.Log.e(TAG, "open Camera on Error =  " + error);
                    camera.close();
                    mCameraDevice = null;
                }
            }, mCaptureHandler);
        } catch (CameraAccessException e) {
            android.util.Log.e(TAG, "Start Capture exception", e);
        } catch (SecurityException e) {
            android.util.Log.e(TAG, "Start Capture exception", e);
        }

//        try {
//            int cameraId = 0;
//            final CameraCharacteristics characteristics = manager.getCameraCharacteristics(cameraId);
//
////            final StreamConfigurationMap map =
////                    characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
//
////            // For still image captures, we use the largest available size.
////            final Size largest =
////                    Collections.max(
////                            Arrays.asList(map.getOutputSizes(ImageFormat.YUV_420_888)),
////                            new CompareSizesByArea());
////
////            sensorOrientation = characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);
//
//            // Danger, W.R.! Attempting to use too large a preview size could  exceed the camera
//            // bus' bandwidth limitation, resulting in gorgeous previews but the storage of
//            // garbage capture data.
//            previewSize =
//                    chooseOptimalSize(map.getOutputSizes(SurfaceTexture.class),
//                            inputSize.getWidth(),
//                            inputSize.getHeight());
//
//            // We fit the aspect ratio of TextureView to the size of preview we picked.
//            final int orientation = getResources().getConfiguration().orientation;
//            if (orientation == Configuration.ORIENTATION_LANDSCAPE) {
//                textureView.setAspectRatio(previewSize.getWidth(), previewSize.getHeight());
//            } else {
//                textureView.setAspectRatio(previewSize.getHeight(), previewSize.getWidth());
//            }
//        } catch (final CameraAccessException e) {
//        }

    }

    private void createCameraPreviewSession() {
        try {
            final SurfaceTexture texture = mTextureView.getSurfaceTexture();

            // We configure the size of default buffer to be the size of camera preview we want.
            texture.setDefaultBufferSize(mPreviewSize.getWidth(), mPreviewSize.getHeight());

            final Surface surface = new Surface(texture);

            // We set up a CaptureRequest.Builder with the output Surface.
            final CaptureRequest.Builder previewRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            previewRequestBuilder.addTarget(surface);

            // Create the reader for the preview frames.
            mImageReader = ImageReader.newInstance(
                    mPreviewSize.getWidth(), mPreviewSize.getHeight(), ImageFormat.YUV_420_888, 2);

            mImageReader.setOnImageAvailableListener(new ImageReader.OnImageAvailableListener() {
                @Override
                public void onImageAvailable(ImageReader reader) {

                    final int previewWidth = mPreviewSize.getWidth();
                    final int previewHeight = mPreviewSize.getHeight();
                    if (previewWidth == 0 || previewHeight == 0) {
                        return;
                    }
                    if (mRgbBytes == null) {
                        mRgbBytes = new byte[previewWidth * previewHeight * 3];
                    }

                    final Image image = reader.acquireLatestImage();
                    if (image == null) return;

                    if (mInProcessing) {
                        image.close();
                        return;
                    }

                    mInProcessing = true;

                    mInferHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            byte[][] yuvBytes = new byte[3][];
                            int yRowStride = 0;

                            final Image.Plane[] planes = image.getPlanes();
                            ImageUtils.fillBytes(planes, yuvBytes);
                            yRowStride = planes[0].getRowStride();
                            final int uvRowStride = planes[1].getRowStride();
                            final int uvPixelStride = planes[1].getPixelStride();

                            ImageUtils.convertYUV420ToARGB8888(
                                    yuvBytes[0],
                                    yuvBytes[1],
                                    yuvBytes[2],
                                    previewWidth,
                                    previewHeight,
                                    yRowStride,
                                    uvRowStride,
                                    uvPixelStride,
                                    mRgbBytes);

                            List<SSDData> results = mImageRecognizer.infer(mRgbBytes, previewHeight, previewWidth, 3, mAccuracyThreshold);
                            mSSDLayerView.populateSSDList(results);

                            image.close();
                            mInProcessing = false;
                        }
                    });
                }
            }, mCaptureHandler);
            previewRequestBuilder.addTarget(mImageReader.getSurface());

            // Here, we create a CameraCaptureSession for camera preview.
            mCameraDevice.createCaptureSession(
                    Arrays.asList(surface, mImageReader.getSurface()),
                    new CameraCaptureSession.StateCallback() {

                        @Override
                        public void onConfigured(final CameraCaptureSession cameraCaptureSession) {
                            // The camera is already closed
                            if (null == mCameraDevice) {
                                return;
                            }

                            // When the session is ready, we start displaying the preview.
                            mCaptureSession = cameraCaptureSession;
                            try {
                                previewRequestBuilder.set(
                                        CaptureRequest.CONTROL_AF_MODE,
                                        CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
                                previewRequestBuilder.set(
                                        CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH);

                                CaptureRequest previewRequest = previewRequestBuilder.build();
                                mCaptureSession.setRepeatingRequest(
                                        previewRequest, new CameraCaptureSession.CaptureCallback() {
                                            @Override
                                            public void onCaptureProgressed(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull CaptureResult partialResult) {
                                                super.onCaptureProgressed(session, request, partialResult);

                                                Log.d(TAG, "onCaptureProgressed");
                                            }

                                            @Override
                                            public void onCaptureFailed(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull CaptureFailure failure) {
                                                super.onCaptureFailed(session, request, failure);
                                                Log.d(TAG, "onCaptureFailed = " + failure.getReason());
                                            }

                                            @Override
                                            public void onCaptureSequenceCompleted(@NonNull CameraCaptureSession session, int sequenceId, long frameNumber) {
                                                super.onCaptureSequenceCompleted(session, sequenceId, frameNumber);
                                                Log.d(TAG, "onCaptureSequenceCompleted");
                                            }
                                        }, mCaptureHandler);
                            } catch (final CameraAccessException e) {
                                Log.e(TAG, "onConfigured exception ", e);
                            }
                        }

                        @Override
                        public void onConfigureFailed(final CameraCaptureSession cameraCaptureSession) {
                            Log.e(TAG, "onConfigureFailed ");
                        }
                    },
                    null);
        } catch (final CameraAccessException e) {
            Log.e(TAG, "createCameraPreviewSession exception ", e);
        }
    }

    private void closeCamera() {
        if (mCaptureSession != null) {
            mCaptureSession.close();
            mCaptureSession = null;
        }
        if (mCameraDevice != null) {
            mCameraDevice.close();
            mCameraDevice = null;
        }
        if (mImageReader != null) {
            mImageReader.close();
            mImageReader = null;
        }
    }

    //------- Settings ------------

    class SettingsViewHolder {

        @BindView(R.id.settingsView)
        View mSettingsView;

        @BindView(R.id.pascalRadioBtn)
        RadioButton mPascalRadioBtn;

        @BindView(R.id.faceRadioBtn)
        RadioButton mFaceRadioBtn;

        @BindView(R.id.backCameraRadioBtn)
        RadioButton mBackCameraRadioBtn;

        @BindView(R.id.frontCameraRadioBtn)
        RadioButton mFrontCameraRadioBtn;

        @BindView(R.id.accuracySlider)
        SeekBar mAccuracySlider;

        @BindView(R.id.accuracyText)
        TextView mAccuracyText;

        @OnCheckedChanged({R.id.backCameraRadioBtn, R.id.frontCameraRadioBtn})
        public void onCameraSelected(CompoundButton button, boolean checked) {

            Log.d("XXX", "onCameraSelected");
            if (checked) {
                switch (button.getId()) {
                    case R.id.backCameraRadioBtn:
                        mBackCamera = true;
                        break;

                    case R.id.frontCameraRadioBtn:
                        mBackCamera = false;
                        break;
                }
                SharedPreferences.Editor editor = mPrefs.edit();
                editor.putBoolean(PREF_KEY_CAMERA, mBackCamera);
                editor.commit();
            }
        }

        @OnCheckedChanged({R.id.pascalRadioBtn, R.id.faceRadioBtn})
        public void onModelSelected(CompoundButton button, boolean checked) {
            if (checked) {
                switch (button.getId()) {
                    case R.id.pascalRadioBtn:
                        mModel = SSDModel.PASCAL_MOBILENET_300;
                        break;

                    case R.id.faceRadioBtn:
                        mModel = SSDModel.FACE_MOBILENET_160;
                        break;
                }
                SharedPreferences.Editor editor = mPrefs.edit();
                editor.putString(PREF_KEY_MODEL, mModel.modelFileName);
                editor.commit();
            }
        }
    }

    private static final String PREF_NAME = "prefs";
    private static final String PREF_KEY_THRESHOLD = "accuracy_threshold";
    private static final String PREF_KEY_MODEL = "ssd_model";
    private static final String PREF_KEY_CAMERA = "camera";


    @OnClick(R.id.ssdLayerView)
    public void onLayerClick(View v) {
        if (!mSettingsShown) {
            LayoutInflater inflater = getLayoutInflater();
            View settingsView = inflater.inflate(R.layout.settings, mSettingsContainer, true);
            mSettingsViewHolder = new SettingsViewHolder();
            ButterKnife.bind(mSettingsViewHolder, settingsView);
            populateSettingsView();
            mSettingsShown = true;
        } else {
            mSettingsContainer.removeAllViews();
            mSettingsShown = false;
        }

    }

    private void loadSettings() {
        mPrefs = getSharedPreferences(PREF_NAME, Context.MODE_PRIVATE);
        mBackCamera = mPrefs.getBoolean(PREF_KEY_CAMERA, true);
        mAccuracyThreshold = mPrefs.getFloat(PREF_KEY_THRESHOLD, 0.5f);
        String modelFileName = mPrefs.getString(PREF_KEY_MODEL, SSDModel.PASCAL_MOBILENET_300.modelFileName);
        mModel = SSDModel.fromModelFileName(modelFileName);
    }

    private void populateSettingsView() {
        switch (mModel) {
            case PASCAL_MOBILENET_300:
                mSettingsViewHolder.mPascalRadioBtn.setChecked(true);
                break;
            case FACE_MOBILENET_160:
                mSettingsViewHolder.mFaceRadioBtn.setChecked(true);
                break;
        }

        if (mBackCamera) {
            mSettingsViewHolder.mBackCameraRadioBtn.setChecked(true);
        } else {
            mSettingsViewHolder.mFrontCameraRadioBtn.setChecked(true);
        }

        mSettingsViewHolder.mAccuracySlider.setProgress((int)(mAccuracyThreshold * 100));
        mSettingsViewHolder.mAccuracyText.setText(mAccuracyThreshold + "");

        mSettingsViewHolder.mAccuracySlider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                mAccuracyThreshold = progress / 10 / 10.f;
                mSettingsViewHolder.mAccuracyText.setText(mAccuracyThreshold + "");

                SharedPreferences.Editor editor = mPrefs.edit();
                editor.putFloat(PREF_KEY_THRESHOLD, mAccuracyThreshold);
                editor.commit();
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
    }



}
