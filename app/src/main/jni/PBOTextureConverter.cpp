#include "PBOTextureConverter.h"

#include <GLES3/gl3.h>

#include <android/log.h>

#include "Utils.h"
#include "GLUtils.h"


bool PBOTextureConverter::convert(cv::Mat& res, cv::Rect region, const GLTextureDescriptor& texture) {
    assert(res.cols != 0 && res.rows != 0 && res.type() == CV_8UC4 &&
           res.size() == region.size());

    __android_log_print(ANDROID_LOG_DEBUG, TAG, "PBOTextureConverter::convert");

    int width  = region.width;
    int height = region.height;

    GLsizeiptr buffer_size = width * height * 4;

    if (!mBufferCreated) {
        OPENGL_CALL(glGenBuffers(1, &mBufferId));
        mBufferCreated = true;
    }

    if (buffer_size != mBufferSize) {
        OPENGL_CALL(glBindBuffer(GL_PIXEL_PACK_BUFFER, mBufferId));
        OPENGL_CALL(glBufferData(GL_PIXEL_PACK_BUFFER, buffer_size, 0, GL_DYNAMIC_READ));
        OPENGL_CALL(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));
        mBufferSize = buffer_size;
    }

    // Set the texture.
    OPENGL_CALL(glActiveTexture(GL_TEXTURE0));

    OPENGL_CALL(glBindTexture(texture.target, texture.id));
    OPENGL_CALL(glBindBuffer(GL_PIXEL_PACK_BUFFER, mBufferId));

    // OPENGL_CALL(glReadBuffer(GL_COLOR_ATTACHMENT0));
    OPENGL_CALL(glReadPixels(region.x, region.y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0));
    void* ptr;
    OPENGL_CAA(ptr, glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, mBufferSize, GL_MAP_READ_BIT));
    memcpy(res.data, ptr, mBufferSize);
    OPENGL_CALL(glUnmapBuffer(GL_PIXEL_PACK_BUFFER));

    OPENGL_CALL(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));
    OPENCV_CALL(glBindTexture(texture.target, 0));

    return true;
}

#if 0
AbstractTextureConverter* AbstractTextureConverter::create() {
    return new PBOTextureConverter;
}
#endif
