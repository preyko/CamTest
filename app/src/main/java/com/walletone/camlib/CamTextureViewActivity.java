/*
 * Copyright 2013 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.walletone.camlib;

import android.app.Activity;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.TextureView;
import android.view.Window;
import android.view.WindowManager;

import com.walletone.camlib.util.FPSCounter;

import java.io.IOException;

import javax.microedition.khronos.opengles.GL10;

/**
 * More or less straight out of TextureView's doc.
 * <p>
 * TODO: add options for different display sizes, frame rates, camera selection, etc.
 */
public class CamTextureViewActivity extends Activity implements TextureView.SurfaceTextureListener {
    private static final String TAG = CamTextureViewActivity.class.getName();

    private Camera mCamera;
    private FPSCounter mFPSCounter = new FPSCounter();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

        TextureView textureView = new TextureView(this) {
            @Override
            protected void onAttachedToWindow() {
                super.onAttachedToWindow();

                Log.d(TAG, "TextureView is Attached");
            }
        };
        textureView.setSurfaceTextureListener(this);

        setContentView(textureView);
    }


    /// callbacks from TextureView
    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        Log.d(TAG, "onSurfaceTextureAvailable");

        // mTextureView.setRotation(-90);

        rebindTextureId(surface);

        mCamera = Camera.open();
        if (mCamera == null) {
            // Seeing this on Nexus 7 2012 -- I guess it wants a rear-facing camera, but
            // there isn't one.  TODO: fix
            throw new RuntimeException("Default camera not available");
        }

        setCameraDisplayOrientation(0, mCamera);

        try {
            Camera.Parameters parameters = mCamera.getParameters();
            //parameters.setPreviewFpsRange(30000, 30000);
            parameters.setRecordingHint(true);
            parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
            mCamera.setParameters(parameters);
        } catch (RuntimeException ex) {
            ex.printStackTrace();
        }

        try {
            Camera.Parameters parameters = mCamera.getParameters();
            parameters.setPreviewFpsRange(20000, 30000);
            // parameters.setPreviewFrameRate(30);
            mCamera.setParameters(parameters );
        } catch (RuntimeException ex) {
            ex.printStackTrace();
        }

        try {
            mCamera.setPreviewTexture(surface);
            mCamera.startPreview();
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        // Ignored, Camera does all the work for us
    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        Log.d(TAG, "onSurfaceTextureDestroyed");

        mCamera.stopPreview();
        mCamera.release();
        return true;
    }

    long mPrevCallbackTime = 0;

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        // Invoked every time there's a new Camera preview frame
        //Log.d(TAG, "updated, ts=" + surface.getTimestamp());
//        if (!mFPSCounter.isStarted())
//            mFPSCounter.start();

        Log.d(TAG, "Time between last callback: " + (System.currentTimeMillis() - mPrevCallbackTime));
        mPrevCallbackTime = System.currentTimeMillis();

        // long start = System.currentTimeMillis();
        // mTextureView.getBitmap();
        // Log.d(TAG, "Time to get bitmap: " + (System.currentTimeMillis() - start));
    }


    /// SurfaceTexture utils
    private void rebindTextureId(SurfaceTexture surface) {
        int new_id = createTexture();

        // surface.detachFromGLContext();
        surface.attachToGLContext(new_id);
    }

    private static int createTexture() {
        int[] textures = new int[1];

        // Generate the texture to where android view will be rendered
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glGenTextures(1, textures, 0);
        checkGlError("Texture generate");

        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, textures[0]);
        checkGlError("Texture bind");

        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_MIN_FILTER,GL10.GL_LINEAR);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_WRAP_S, GL10.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_WRAP_T, GL10.GL_CLAMP_TO_EDGE);

        return textures[0];
    }

    public static void checkGlError(String op) {
        int error;
        while ((error = GLES20.glGetError()) != GLES20.GL_NO_ERROR) {
            Log.e(TAG, op + ": glError " + GLUtils.getEGLErrorString(error));
        }
    }


    public void setCameraDisplayOrientation(int cameraId, android.hardware.Camera camera) {
        Camera.CameraInfo info = new android.hardware.Camera.CameraInfo();
        Camera.getCameraInfo(cameraId, info);
        int rotation = getWindowManager().getDefaultDisplay().getRotation();
        int degrees = 0;
        switch (rotation) {
            case Surface.ROTATION_0: degrees = 0; break;
            case Surface.ROTATION_90: degrees = 90; break;
            case Surface.ROTATION_180: degrees = 180; break;
            case Surface.ROTATION_270: degrees = 270; break;
        }

        int result;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            result = (info.orientation + degrees) % 360;
            result = (360 - result) % 360;  // compensate the mirror
        } else {  // back-facing
            result = (info.orientation - degrees + 360) % 360;
        }
        camera.setDisplayOrientation(result);
    }
}