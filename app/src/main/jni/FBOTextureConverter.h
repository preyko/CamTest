#ifndef FBOTEXTURECONVERTER_H
#define FBOTEXTURECONVERTER_H

#include <AbstractTextureConverter.h>


class FBOTextureConverter : public AbstractTextureConverter {
public:
    bool convert(cv::Mat& res, cv::Rect region, const GLTextureDescriptor& texture) override;

private:
    // Current texture descriptor
    GLTextureDescriptor mCurrentTexture { -1, -1 };

    // Buffer stuff
    uint mFramebuffer = 0;

};

#endif // FBOTEXTURECONVERTER_H
