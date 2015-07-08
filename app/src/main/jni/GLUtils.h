#pragma once

#include <android/log.h>

#include "Utils.h"

inline
void CheckOpenGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::string error;

        switch(err) {
                case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
                case GL_INVALID_ENUM:      error = "INVALID_ENUM";      break;
                case GL_INVALID_VALUE:     error = "INVALID_VALUE";     break;
                case GL_OUT_OF_MEMORY:     error = "OUT_OF_MEMORY";     break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION";  break;
                default: error = "UNDEFINED"; break;
        }
        __android_log_print(ANDROID_LOG_ERROR, TAG,
                            "OpenGL error %s(%08x), at %s:%i - for %s\n", error.c_str(), err, fname, line, stmt);
    }
}

#define OPENGL_CALL(stmt) do { \
    stmt; \
    CheckOpenGLError(#stmt, __FILE__, __LINE__); \
} while (0)

#define OPENGL_CAA(lv, stmt) do { \
    (lv) = (stmt); \
    CheckOpenGLError(#stmt, __FILE__, __LINE__); \
} while (0)
