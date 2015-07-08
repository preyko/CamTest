#include "EGLITextureConverter.h"

#include <stdexcept>

#include <android/log.h>

#if 0
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <system/window.h>

#include <ui/GraphicBuffer.h>
#endif

#include "Utils.h"


bool EGLICamTextureConverter::convert(cv::Mat& res, cv::Rect region, const GLTextureDescriptor& texture) {
    //TODO Implement
    __android_log_print(ANDROID_LOG_ERROR, TAG, "EGLICamTextureConverter::convert not implemented!");
    return false;

    #if 0
    cv::Mat texture_img(height, width, CV_8UC4);

    android::sp<android::GraphicBuffer> window(new android::GraphicBuffer(
                                               width, height,
                                               android::PIXEL_FORMAT_RGBA_8888,
                                               android::GraphicBuffer::USAGE_SW_READ_OFTEN |
                                               android::GraphicBuffer::USAGE_HW_TEXTURE));

    // ANativeWindowBuffer* buffer = window.getNativeBuffer();


    const EGLint renderImageAttrs[] = {
            EGL_WIDTH,               width,
            EGL_HEIGHT,              height,
            EGL_MATCH_FORMAT_KHR,    EGL_FORMAT_RGBA_8888_KHR,
            EGL_IMAGE_PRESERVED_KHR, EGL_TRUE,
            EGL_NONE
    };

    EGLImageKHR* image = eglCreateImageKHR(eglGetCurrentDisplay(), EGL_NO_CONTEXT,
                                           reinterpret_cast<EGLClientBuffer>(EGL_NATIVE_BUFFER_ANDROID),
                                           renderImageAttrs);

    uchar* ptr;
    glBindTexture(GL_TEXTURE_2D, textureId);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image);

    window->lock(android::GraphicBuffer::USAGE_SW_READ_OFTEN, &ptr);
    memcpy(texture_img.data, ptr, width * height * 4);
    window->unlock();

    return texture_img;
    #endif
}

/*
AbstractCamTextureConverter* AbstractCamTextureConverter::create() {
    return new EGLICamTextureConverter;
}
*/
