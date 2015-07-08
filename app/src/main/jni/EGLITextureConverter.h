#pragma once

#include "AbstractTextureConverter.h"

/**
 * @brief The EGLICamTextureConverter class
 *  This class convert OGL texture to image using EGLImage.
 *  For more information visit
 *  https://vec.io/posts/faster-alternatives-to-glreadpixels-and-glteximage2d-in-opengl-es
 *
 *  **** NOT IMPLEMENTED ****
 */
class EGLICamTextureConverter : public AbstractTextureConverter {
public:
    bool convert(cv::Mat& res, cv::Rect region, const GLTextureDescriptor& texture) override;
};
