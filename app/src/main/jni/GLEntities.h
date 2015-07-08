#pragma once

#include <array>


/**
 * @brief The GLTextureDescriptor struct which keeps all necessary parameters of  texture
 */
struct GLTextureDescriptor {
    /**
     * @brief id - texture id
     */
    int id;

    /**
     * @brief target
     */
    int target;

    /**
     * @brief transform - texture transform matrix.
     * See http://developer.android.com/reference/android/graphics/SurfaceTexture.html#getTransformMatrix(float%5B%5D)
     * for more information
     */
    std::array<float, 16> transform;
};
