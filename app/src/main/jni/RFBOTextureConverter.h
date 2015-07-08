#pragma once

#include "AbstractTextureConverter.h"


/**
 * @brief The RFBTextureConverter class
 *  This class convert OGL texture to image using Frame Buffer in GLES2.
 *  It renders texture into another texture using Frame Buffer then read pixels from this texture.
 *  For more information visit
 *  github.com/chrisbatt/AndroidFastImageProcessing/blob/master/framework/FastImageProcessing/src/project/android/imageprocessing/output/BitmapOutput.java
 */
class RFBOTextureConverter : public AbstractTextureConverter {
public:
    RFBOTextureConverter();
    ~RFBOTextureConverter();

    bool convert(cv::Mat& res, cv::Rect region, const GLTextureDescriptor& texture) override;

private:
    // Main init
    void init();

    bool mInitialized = false;

    // Vertices
    float mTextureVertices[8] {
         0.0f, 1.0f,
         1.0f, 1.0f,
         0.0f, 0.0f,
         1.0f, 0.0f,
    };

    float mRenderVertices[8] {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    // Shader handles
    uint mProgramHandle;

    uint mVertexShaderHandle;
    uint mFragmentShaderHandle;
    uint mTextureHandle;
    uint mPositionHandle;
    uint mTexCoordHandle;

    // FBO
    void initFBO();

    // Input texture
    GLTextureDescriptor mInputTexture;

    // Output texture
    cv::Size mOutputSize;
    uint     mOutputTexture;

    // Buffers
    uint mFrameBuffer;
    uint mDepthRenderBuffer;
};
