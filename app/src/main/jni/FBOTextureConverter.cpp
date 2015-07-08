#include "FBOTextureConverter.h"

#include <GLES2/gl2.h>

#include "GLUtils.h"


bool FBOTextureConverter::convert(cv::Mat& res, cv::Rect region, const GLTextureDescriptor& texture) {
    assert(res.cols != 0 && res.rows != 0 && res.type() == CV_8UC4 &&
           res.size() == region.size());

    __android_log_print(ANDROID_LOG_DEBUG, TAG, "FBOTextureConverter::convert");

    if (mFramebuffer == 0) {
        OPENGL_CALL(glGenFramebuffers(1, &mFramebuffer));

        // OPENGL_CALL(glBindFramebufferOES(GL_FRAMEBUFFER, mFramebuffer));
    }

    if (texture.id != mCurrentTexture.id) {
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);

        OPENGL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.target, texture.id, 0));

        ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, nullptr);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        mCurrentTexture = texture;
    }

    OPENGL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer));

    OPENGL_CALL(glReadPixels(region.x, region.y, region.width, region.height, GL_ACTIVE_ATTRIBUTES, GL_UNSIGNED_BYTE, res.data));

    OPENGL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    return true;
}
