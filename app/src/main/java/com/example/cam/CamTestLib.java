package com.example.cam;

/**
 * Created by Daniel Daniel's on 29.06.15.
 * Mail to: danields761@gmail.com
 */
public class CamTestLib {
    public enum TextureConvertMethod {
        /**
         * Frame Buffer Object method.
         */
        FBO,

        /**
         * Pixel Buffer Object method. GLES3 is required.
         */
        PBO,

        /**
         * Render in FBO method.
         */
        RFBO
    }

    public enum TextureConvertMode {
        LRTBStride,
        Card,
        FullFrame
    }

    public static class GlTexture {

        public GlTexture(int i, int t) {
            this(i, t, null);
        }

        public GlTexture(int i, int t, float[] tm) {
            id = i;
            target = t;
            transform_matrix = tm;
        }
        public int id;

        public int target;
        public float[] transform_matrix;

    }

    public static void setTextureConvertMethod(TextureConvertMethod method) {
        nativeSetTextureSaveMethod(method.ordinal());
    }

    public static void setTextureConvertMode(TextureConvertMode mode) {
        nativeSetConvertMode(mode.ordinal());
    }

    native private static void nativeSetTextureSaveMethod(int method);

    native private static void nativeSetConvertMode(int convert_mode);

    native public static void nativeSetImageSavePath(String path);

    native public static void nativeSaveTexture(GlTexture texture, int width, int height, boolean saveFrame);

    static {
        System.loadLibrary("camtest");
    }
}
