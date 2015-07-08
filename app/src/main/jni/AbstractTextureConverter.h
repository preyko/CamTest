#pragma once

#include <array>

#include <opencv2/opencv.hpp>

#include "GLEntities.h"


/**
 * @brief The AbstractTextureConverter class
 * Abstract class which encapsulates conversion details from user.
 */
class AbstractTextureConverter {
public:
    /**
     * @brief ~AbstractTextureConverter - virtual destructor
     */
    virtual ~AbstractTextureConverter() {
    }

    /**
     * @brief prepare prepare converter stuff to work.
     * For example prepare opengl context, or buffer objects etc.
     */
    virtual void prepare() {
    }

    /**
     * @brief convert - method, which perform converting of a texture region into image
     * @param res - pre-initialized cv::Mat with correct size and CV_8UC4 type
     * @param region - desired area of texture to convert. Note: res.size() == region.size()
     * @param texture - texture descriptor which should keeps all valid params
     * @return true on success, false - otherwise
     */
    virtual bool convert(cv::Mat& res, cv::Rect region, const GLTextureDescriptor& texture) = 0;

    /**
     * @brief create - create default subclass of AbstractTextureConverter
     * @return default AbstractTextureConverter
     */
    static AbstractTextureConverter* create();
};
