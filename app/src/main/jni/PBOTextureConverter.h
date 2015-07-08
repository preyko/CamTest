#pragma once

#include "AbstractTextureConverter.h"


/**
 * @brief The PBOTextureConverter class
 *  This class convert OGL texture to image using Pixel Pack Buffer(PBO) in GLES 3.
 *  For more information visit
 *  vec.io/posts/faster-alternatives-to-glreadpixels-and-glteximage2d-in-opengl-es
 */
class PBOTextureConverter : public AbstractTextureConverter {
public:
    bool convert(cv::Mat& res, cv::Rect region, const GLTextureDescriptor& texture) override;

private:
    // GLES3 stuff
    bool mBufferCreated = false;
    uint mBufferId;
    int  mBufferSize = 0;
};
