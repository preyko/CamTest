/*
 * Copyright 2014 Google Inc. All rights reserved.
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
import android.hardware.Camera;
import android.util.Log;
import android.view.Surface;

import java.util.Collections;
import java.util.List;

/**
 * Camera-related utility functions.
 */
public class CameraUtils {
    private static final String TAG = CameraUtils.class.getName();

    /**
     * Compares two sizes
     * @param s1 - first size
     * @param s2 - second size
     * @return 1 if s1 > s2, 0 if equal, -1 if s1 < s2
     */
    private static int compare(Camera.Size s1, Camera.Size s2) {
        int s1a = s1.width * s1.height;
        int s2a = s2.width * s2.height;
        if (s1.equals(s2))
            return 0;
        else
            return s1a > s2a ? 1 : -1;
    }

    /**
     * Attempts to find a preview size that matches the provided width and height (which
     * specify the dimensions of the encoded video).  If it fails to find a match it just
     * uses the default preview size for video.
     * <p>
     * TODO: should do a best-fit match, e.g.
     * https://github.com/commonsguy/cwac-camera/blob/master/camera/src/com/commonsware/cwac/camera/CameraUtils.java
     */
    public static void choosePreviewSize(Camera.Parameters parms, int width, int height) {
        // We should make sure that the requested MPEG size is less than the preferred
        // size, and has the same aspect ratio.
        Camera.Size ppsfv = parms.getPreferredPreviewSizeForVideo();
        if (ppsfv != null) {
            Log.d(TAG, "Camera preferred preview size for video is " +
                    ppsfv.width + "x" + ppsfv.height);
        }

        int best_diff = Integer.MAX_VALUE;
        Camera.Size result = null;
        for (Camera.Size size : parms.getSupportedPreviewSizes()) {
            int diff = size.width * size.height - width * height;
            if (size.width >= width && size.height >= height && diff < best_diff) {
                result = size;
                best_diff = diff;
            }
        }

        if (result != null) {
            parms.setPreviewSize(result.width, result.height);
            return;
        }

        // Try to find nearest preview size
        for (Camera.Size size : parms.getSupportedPreviewSizes()) {
            if (size.width <= width && size.height <= height) {
                if (result == null) {
                    result = size;
                } else {
                    int result_area = result.width * result.height;
                    int new_area = size.width * size.height;

                    if (new_area > result_area) {
                        result = size;
                    }
                }
            }
        }

        if (result != null) {
            parms.setPreviewSize(result.width, result.height);
            return;
        }

        Log.w(TAG, "Unable to set preview size to " + width + "x" + height);
        if (ppsfv != null) {
            parms.setPreviewSize(ppsfv.width, ppsfv.height);
        }
        // else use whatever the default size is
    }

    /**
     * Attempts to find a fixed preview frame rate that matches the desired frame rate.
     * <p>
     * It doesn't seem like there's a great deal of flexibility here.
     * <p>
     * TODO: follow the recipe from http://stackoverflow.com/questions/22639336/#22645327
     *
     * @return The expected frame rate, in thousands of frames per second.
     */
    public static int chooseFixedPreviewFps(Camera.Parameters parms, int desiredThousandFps) {
        List<int[]> supported = parms.getSupportedPreviewFpsRange();

        for (int[] entry : supported) {
            //Log.d(TAG, "entry: " + entry[0] + " - " + entry[1]);
            if ((entry[0] == entry[1]) && (entry[0] == desiredThousandFps)) {
                parms.setPreviewFpsRange(entry[0], entry[1]);
                return entry[0];
            }
        }

        int[] tmp = new int[2];
        parms.getPreviewFpsRange(tmp);
        int guess;
        if (tmp[0] == tmp[1]) {
            guess = tmp[0];
        } else {
            guess = tmp[1] / 2;     // shrug
        }

        Log.d(TAG, "Couldn't find match for " + desiredThousandFps + ", using " + guess);
        return guess;
    }

    /**
     * This function attempts to find and set FPS range, where a bottom bound of FPS range is
     *  greater than minimumFps parameter. If it not found, then try to find fixed FPS by old
     *  Camera API
     * @param parameters - Camera parameters
     * @param minimumFps - bottom bound of FPS
     * @return -1 - if desired FPS range is set, fps values - if fixed (old API) fps is set, 0 - failed to
     * set desired fps
     */
    public static int chooseCameraFpsMoreThen(Camera.Parameters parameters, int minimumFps) {
        int[] desiredRange = null;
        List<int[]> supportedRanges = parameters.getSupportedPreviewFpsRange();
        for (int[] range : supportedRanges)
            if (range[0] >= minimumFps)
                if (desiredRange != null) {
                    if (desiredRange[0] < range[0])
                        desiredRange = range;
                } else {
                    desiredRange = range;
                }

        if (desiredRange != null) {
            parameters.setPreviewFpsRange(desiredRange[0], desiredRange[1]);
            return -1;
        }

        List<Integer> supportedFps = parameters.getSupportedPreviewFrameRates();
        Integer maxFps = Collections.max(supportedFps);
        if (maxFps > minimumFps) {
            parameters.setPreviewFrameRate(maxFps);
            return maxFps;
        }

        return 0;
    }

    /**
     * Attempts to set camera orientation to make it horizontal in current activity
     * @param activity - current activity
     * @param cameraId - id of the camera
     * @param camera - Camera object
     */
    public static void chooseCameraDisplayOrientation(Activity activity, int cameraId, Camera camera) {
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(cameraId, info);

        int rotation = activity.getWindowManager().getDefaultDisplay().getRotation();
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
