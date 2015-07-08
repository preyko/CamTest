#include "RFBOTextureConverter.h"

#include <GLES2/gl2.h>

#include "Utils.h"
#include "GLUtils.h"



#define ATTRIBUTE_POSITION  "a_Position"
#define ATTRIBUTE_TEXCOORD  "a_TexCoord"
#define VARYING_TEXCOORD    "v_TexCoord"
#define UNIFORM_TEXTUREBASE "u_Texture"
#define UNIFORM_TEXTURE0    "u_Texture0"


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
const char* VERTEX_SHADER = "attribute vec4 " ATTRIBUTE_POSITION ";\n"
                            "attribute vec2 " ATTRIBUTE_TEXCOORD ";\n"
                            "varying vec2 " VARYING_TEXCOORD ";\n"

                            "void main() {\n"
                            "  " VARYING_TEXCOORD " = " ATTRIBUTE_TEXCOORD ";\n"
                            "   gl_Position = " ATTRIBUTE_POSITION ";\n"
                            "}\n";

const char* FRAGMENT_SHADER = "precision mediump float;\n"
                              "uniform sampler2D " UNIFORM_TEXTURE0 ";\n"
                              "varying vec2 " VARYING_TEXCOORD ";\n"
                              "void main(){\n"
                              "   gl_FragColor = texture2D(" UNIFORM_TEXTURE0 "," VARYING_TEXCOORD ");\n"
                              "}\n";


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
std::string getShaderInfoLog(GLuint handle) {
    int length;
    OPENGL_CALL(glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length));

    std::string log((size_t) length + 1, 'a');

    int writed_length;
    OPENGL_CALL(glGetShaderInfoLog(handle, log.length(), &writed_length, &(log[0])));

    return log;
}

bool compileShader(GLuint handle, const std::string& code, std::string& error) {
    const char* c = code.c_str();
    int l = code.length();
    OPENGL_CALL(glShaderSource(handle, 1, &c, &l));

    OPENGL_CALL(glCompileShader(handle));

    int compile_status;
    OPENGL_CALL(glGetShaderiv(handle, GL_COMPILE_STATUS, &compile_status));
    LOGV("Compiling shader, status: %d", compile_status);

    if (compile_status == 0) {
        error = getShaderInfoLog(handle);

        OPENGL_CALL(glDeleteShader(handle));

        return false;
    }

    return true;
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
RFBOTextureConverter::RFBOTextureConverter() {

}

RFBOTextureConverter::~RFBOTextureConverter() {
    if(mProgramHandle != 0) {
        glDeleteProgram(mProgramHandle);
    }
    if(mVertexShaderHandle != 0) {
        glDeleteShader(mVertexShaderHandle);
    }
    if(mFragmentShaderHandle != 0) {
        glDeleteShader(mFragmentShaderHandle);
    }
}

void RFBOTextureConverter::init() {
    LOGV("Initalizing RFBOTextureConverter...");

    std::string error = "Can't create shader.";

    OPENGL_CAA(mVertexShaderHandle, glCreateShader(GL_VERTEX_SHADER));
    LOGV("Creating vertex shader ...");
    if (mVertexShaderHandle != 0) {
        if (!compileShader(mVertexShaderHandle, VERTEX_SHADER, error)) {
            mVertexShaderHandle = 0;
        }
    }
    ASSERT(mVertexShaderHandle != 0, "Could not create vertex shader. Reason: %s", error.c_str());


    OPENGL_CAA(mFragmentShaderHandle, glCreateShader(GL_FRAGMENT_SHADER));
    LOGV("Creating fragment shader ...");
    if (mFragmentShaderHandle != 0) {
        if (!compileShader(mFragmentShaderHandle, FRAGMENT_SHADER, error)) {
            mFragmentShaderHandle = 0;
        }
    }

    ASSERT(mFragmentShaderHandle != 0, "Could not create fragment shader. Reason: %s", error.c_str());


    OPENGL_CAA(mProgramHandle, glCreateProgram());
    if (mProgramHandle != 0) {
        OPENGL_CALL(glAttachShader(mProgramHandle, mVertexShaderHandle));
        OPENGL_CALL(glAttachShader(mProgramHandle, mFragmentShaderHandle));

        OPENGL_CALL(glBindAttribLocation(mProgramHandle, 0, ATTRIBUTE_POSITION));
        OPENGL_CALL(glBindAttribLocation(mProgramHandle, 1, ATTRIBUTE_TEXCOORD));

        OPENGL_CALL(glLinkProgram(mProgramHandle));

        int link_status;
        OPENGL_CALL(glGetProgramiv(mProgramHandle, GL_LINK_STATUS, &link_status));

        if (link_status == 0) {
            OPENGL_CALL(glDeleteProgram(mProgramHandle));
            mProgramHandle = 0;
        }
    }

    ASSERT(mProgramHandle != 0, nullptr);


    mTextureHandle  = glGetUniformLocation(mProgramHandle, UNIFORM_TEXTURE0);
    mPositionHandle = glGetAttribLocation(mProgramHandle, ATTRIBUTE_POSITION);
    mTexCoordHandle = glGetAttribLocation(mProgramHandle, ATTRIBUTE_TEXCOORD);

    mInitialized = true;

    LOGV("Initializing is complited!");
}

void RFBOTextureConverter::initFBO() {
    LOGV("Initializing RFBOTextureConverter FBO.");

    if (mFrameBuffer != 0) {
        glDeleteFramebuffers(1, &mFrameBuffer);
        mFrameBuffer = 0;
    }

    if (mOutputTexture != 0) {
        glDeleteTextures(1, &mOutputTexture);
        mOutputTexture = 0;
    }

    if (mDepthRenderBuffer != 0) {
        glDeleteRenderbuffers(1, &mDepthRenderBuffer);
        mDepthRenderBuffer = 0;
    }

    OPENGL_CALL(glGenFramebuffers(1, &mFrameBuffer));
    OPENGL_CALL(glGenRenderbuffers(1, &mDepthRenderBuffer));
    OPENGL_CALL(glGenTextures(1, &mOutputTexture));

    OPENGL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer));

    OPENGL_CALL(glActiveTexture(GL_TEXTURE0));
    OPENGL_CALL(glBindTexture(GL_TEXTURE_2D, mOutputTexture));
    OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mOutputSize.width, mOutputSize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    OPENGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    OPENGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    OPENGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    OPENGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    OPENGL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mOutputTexture, 0));

    OPENGL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderBuffer));
    OPENGL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mOutputSize.width, mOutputSize.height));
    OPENGL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer));

    uint status;
    ASSERT((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) == GL_FRAMEBUFFER_COMPLETE,
           "Failed to set up render buffer with status %d.", status);
}

bool RFBOTextureConverter::convert(cv::Mat& res, cv::Rect region, const GLTextureDescriptor& texture) {
    assert(res.cols != 0 && res.rows != 0 && res.type() == CV_8UC4 &&
           res.size() == region.size());

    __android_log_print(ANDROID_LOG_DEBUG, TAG, "RFBOTextureConverter::convert");

    if (!mInitialized)
        init();

    if (region.size() != mOutputSize) {
        mOutputSize = region.size();
        initFBO();
    }

    // Bind framebuffer to draw on it
    OPENGL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer));

    // setup and clear output
    OPENGL_CALL(glViewport(0, 0, mOutputSize.width, mOutputSize.height));
    OPENGL_CALL(glUseProgram(mProgramHandle));

    OPENGL_CALL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
    OPENGL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

    // init shader inputs
    OPENGL_CALL(glVertexAttribPointer(mPositionHandle, 2, GL_FLOAT, false, 8, mRenderVertices));
    OPENGL_CALL(glEnableVertexAttribArray(mPositionHandle));

    OPENGL_CALL(glVertexAttribPointer(mTexCoordHandle, 2, GL_FLOAT, false, 8, mTextureVertices));
    OPENGL_CALL(glEnableVertexAttribArray(mTexCoordHandle));

    OPENGL_CALL(glActiveTexture(GL_TEXTURE1));
    OPENGL_CALL(glBindTexture(texture.target, texture.id));
    OPENGL_CALL(glUniform1i(mTextureHandle, 0));

    // Draw
    OPENGL_CALL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

    // Read pixels from resulting image
    OPENGL_CALL(glReadPixels(region.x, region.y, region.width, region.height, GL_RGBA, GL_UNSIGNED_BYTE, res.data));

    OPENGL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

#if 0
AbstractTextureConverter* AbstractTextureConverter::create() {
    return new RFBOTextureConverter;
}
#endif
