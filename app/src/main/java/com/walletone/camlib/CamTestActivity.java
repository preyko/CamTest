package com.walletone.camlib;

import android.app.Activity;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.view.SurfaceView;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.Toast;

import com.walletone.camlib.util.FPSCounter;

import java.io.File;


/**
 * Created by Daniel Daniel's on 29.06.15.
 * Mail to: danields761@gmail.com
 */
public class CamTestActivity extends Activity {
    private static final String TAG = "CamTestActivity";


    long mPrevCallbackTime = 0;

    Camera.PreviewCallback mPreviewCallback = new Camera.PreviewCallback() {
        @Override
        public void onPreviewFrame(byte[] bytes, Camera camera) {
            // if (!mFPSCounter.isStarted())
            //    mFPSCounter.start();

            // long startTime = System.currentTimeMillis();

//            CamTestLib.nativeSetTimeStamp(System.nanoTime());
//
//            CamTestLib.nativeSaveImage(mBuffer, camera.getParameters().getPreviewSize().width,
//                    camera.getParameters().getPreviewSize().height,
//                    camera.getParameters().getPreviewFormat());
//
            mCamera.addCallbackBuffer(mBuffer);

            // Log.d(TAG, "onPreviewFrame: Time elapsed: " + (System.currentTimeMillis() - startTime));

//            mFPSCounter.tick();
//            Log.d(TAG, "Current FPS: " + mFPSCounter.fpsMs());
            Log.d(TAG, "Time between last callback: " + (System.currentTimeMillis() - mPrevCallbackTime));
            mPrevCallbackTime = System.currentTimeMillis();
        }
    };


    Preview mPreview;
    Camera mCamera;

    byte[] mBuffer;

    FPSCounter mFPSCounter = new FPSCounter();

    final HandlerThread mCameraThread = new HandlerThread("Camera thread") {
        @Override
        protected void onLooperPrepared() {
            super.onLooperPrepared();

            mCameraThreadHandler = new Handler(mCameraThread.getLooper());
        }
    };
    Handler mCameraThreadHandler;

    Handler mMainThreadHandler;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mCameraThread.start();

        mMainThreadHandler = new Handler(getMainLooper());


        CamTestLib.nativeSetImageSavePath(createDir("camtest/native/").getAbsolutePath());

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.main);

        mPreview = new Preview(this, (SurfaceView) findViewById(R.id.surfaceView));
        mPreview.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        ((FrameLayout) findViewById(R.id.layout)).addView(mPreview);
        mPreview.setKeepScreenOn(true);

        mCameraThreadHandler.post(new Runnable() {
            @Override
            public void run() {
                openCam();
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();

        // openCam();
    }

    @Override
    protected void onPause() {
        super.onPause();

        if (mCamera != null) {
            mCamera.stopPreview();
            mPreview.setCamera(null);
            mCamera.release();
            mCamera = null;
        }
    }

    private void openCam() {
        int numCams = Camera.getNumberOfCameras();
        if (numCams > 0) {
            try {
                mCamera = Camera.open(0);

                mMainThreadHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        prepareCam();
                    }
                });

                return;
            } catch (RuntimeException ex) {
                ex.printStackTrace();
            }
        }

        Toast.makeText(CamTestActivity.this, "Camera not found", Toast.LENGTH_LONG).show();
    }

    private void prepareCam() {
        {
            Camera.Parameters parameters = mCamera.getParameters();
            // parameters.setPreviewFpsRange(30000, 30000);
            parameters.setRecordingHint(true);
            parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
            mCamera.setParameters(parameters);
        }
        {
            Camera.Parameters parameters = mCamera.getParameters();
            parameters.setPreviewFpsRange(20000, 30000);
//            parameters.setPreviewFrameRate(30);
            mCamera.setParameters(parameters);
        }

        mPreview.setCamera(mCamera);

        mCamera.startPreview();

        int bufferSize =
                mCamera.getParameters().getPreviewSize().width *
                mCamera.getParameters().getPreviewSize().height *
                ImageFormat.getBitsPerPixel(mCamera.getParameters().getPreviewFormat()) / 8;

        mBuffer = new byte[bufferSize];
        mCamera.addCallbackBuffer(mBuffer);

        mCamera.setPreviewCallbackWithBuffer(mPreviewCallback);
//        mCamera.setPreviewCallback(mPreviewCallback);
    }

    private File createDir(String dirName) {
        File sdCard = Environment.getExternalStorageDirectory();
        File dir = new File(sdCard.getAbsolutePath() + "/" + dirName);

        // if (!dir.mkdirs()) {
        //    Log.d(TAG, "Failed to create dir " + dir.getAbsolutePath());
        //    return null;
        // }
        dir.mkdirs();

        return dir;
    }
}


