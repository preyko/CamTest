#include <string>
#include <cstring>
#include <chrono>
#include <sstream>
#include <memory>

#include <GLES3/gl3.h>

#include <android/log.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "jh/CamTestLib.h"

#include "Utils.h"
#include "GLUtils.h"
#include "JNIUtils.h"

#include "GLEntities.h"

#include "AbstractTextureConverter.h"
#include "RFBOTextureConverter.h"
#include "PBOTextureConverter.h"
#include "FBOTextureConverter.h"


///////////////////////////////////////////////////////////
/////////////////////// DEFINES ///////////////////////////
const char* kCamTestLib_GlTexture_class_name = "com/walletone/camlib/CamTestLib$GlTexture";

enum CamTestLib_TextureSaveMethod {
    FBO, PBO, RFBO
};

enum ExtractType {
    LRTBStripe = 0, CardRect = 1, FullFrame = 2
};

static const float REL_PROP = 0.2f;
static const int   STRIPE_THICKNESS = 16;



///////////////////////////////////////////////////////////
/////////////////////// GLOBALS ///////////////////////////
static std::string gImageSavePath;

static std::unique_ptr<AbstractTextureConverter> gCurrentConverter;

struct {
    jfieldID textureIdField;
    jfieldID textureTargetField;
    jfieldID textureTransform;
} gCamTestLib_GlTexture_fields;

struct {
    // Keep size input for detection of the moment when we should reset converters
    cv::Size mInputSize;

    RFBOTextureConverter mLRStripeConverter; // Used for converting left and rigth stripes
    RFBOTextureConverter mTBStripeConverter; // Used for converting top and bottom stripes

    RFBOTextureConverter mCardRectConverter; // Used for card rect converting

    void calcRegions() {
        cv::Point size { mInputSize.width, mInputSize.height };
        cv::Point lt = size * REL_PROP;
        cv::Point rb = size * (1 - REL_PROP);
        cv::Point rt = lt + cv::Point(mInputSize.width * (1 - 2 * REL_PROP),  0);
        cv::Point lb = lt + cv::Point(0, mInputSize.height * (1 - 2 * REL_PROP));

        mCardRect = cv::Rect(lt, rb);

        mLeftStripe   = cv::Rect(lt, cv::Size(STRIPE_THICKNESS, mCardRect.height));
        mRightStripe  = cv::Rect(rt - cv::Point(STRIPE_THICKNESS, 0), cv::Size(STRIPE_THICKNESS, mCardRect.height));
        mTopStripe    = cv::Rect(lt, cv::Size(mCardRect.width, STRIPE_THICKNESS));
        mBottomStripe = cv::Rect(lb - cv::Point(0, STRIPE_THICKNESS), cv::Size(mCardRect.width, STRIPE_THICKNESS));
    }

    // Stripes regions
    cv::Rect mLeftStripe;
    cv::Rect mRightStripe;
    cv::Rect mTopStripe;
    cv::Rect mBottomStripe;

    // Card rect
    cv::Rect mCardRect;

    // Current extract type
    ExtractType mCurrentType = LRTBStripe;
} state;

void save_image_with_random_name(const cv::Mat& img) {
    std::stringstream ss;
    ss << gImageSavePath << "/" << std::time(nullptr) << ".jpg";

    if (!cv::imwrite(ss.str(), img))
        LOGW("Failed to write image to file %s.", ss.str().c_str());
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
jint JNI_OnLoad(JavaVM* vm, void*) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "Failed to get the environment");
        return -1;
    }

    // Get CamTestLib.GlTexture class
    jclass gltexture_class = env->FindClass(kCamTestLib_GlTexture_class_name);

    if (!gltexture_class) {
        LOGD("failed to get %s class reference", kCamTestLib_GlTexture_class_name);
        return -1;
    }

    gCamTestLib_GlTexture_fields.textureIdField     = env->GetFieldID(gltexture_class, "id",     "I");
    gCamTestLib_GlTexture_fields.textureTargetField = env->GetFieldID(gltexture_class, "target", "I");
    gCamTestLib_GlTexture_fields.textureTransform   = env->GetFieldID(gltexture_class, "transform_matrix", "[F");

    return JNI_VERSION_1_6;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
JNIEXPORT void JNICALL Java_com_walletone_camlib_CamTestLib_nativeSetImageSavePath
  (JNIEnv* env, jclass c, jstring path) {
    gImageSavePath = as_std_string(env, path);

    __android_log_print(ANDROID_LOG_INFO, TAG, "New image save path: %s", gImageSavePath.c_str());
}

JNIEXPORT void JNICALL Java_com_walletone_camlib_CamTestLib_nativeSetTextureSaveMethod
  (JNIEnv* env, jclass c, jint method) {

    std::string name;
    switch ((CamTestLib_TextureSaveMethod) method) {
    case FBO:
        gCurrentConverter.reset(new FBOTextureConverter);
        name = "FBO method";
        break;
    #if 0
    case PBO:
        gCurrentConverter.reset(new PBOTextureConverter);
        name = "PBO method";
        break;
    #endif
    case RFBO:
        gCurrentConverter.reset(new RFBOTextureConverter);
        name = "RFBO method";
        break;
    default:
        assert(false);
        break;
    }

    LOGD("New convert method is set: %s", name.c_str());
}

JNIEXPORT void JNICALL Java_com_walletone_camlib_CamTestLib_nativeSetConvertMode
  (JNIEnv* env, jclass c, jint mode) {
    state.mCurrentType = (ExtractType) mode;

    std::string name;
    switch (mode) {
    case LRTBStripe:
        name = "LRTB Stripe";
        break;
    case CardRect:
        name = "CardRect";
        break;
    case FullFrame:
        name = "FullFrame";
        break;
    default:
        assert(false);
        break;
    }

    LOGD("New convert mode is set: %s", name.c_str());
}

JNIEXPORT void JNICALL Java_com_walletone_camlib_CamTestLib_nativeSaveTexture
  (JNIEnv* env, jclass c, jobject o, jint width, jint height, jboolean save_image) {
    ASSERT(gCurrentConverter.get() != nullptr, "Failed to save texture: call CamTestLib.setTextureSaveMethod() before!");

    int texture_id = env->GetIntField(o, gCamTestLib_GlTexture_fields.textureIdField);
    int target_id  = env->GetIntField(o, gCamTestLib_GlTexture_fields.textureTargetField);

    GLTextureDescriptor texture { texture_id, target_id };

    if (state.mInputSize != cv::Size(width, height)) {
        state.mLRStripeConverter = RFBOTextureConverter();
        state.mTBStripeConverter = RFBOTextureConverter();

        state.mCardRectConverter = RFBOTextureConverter();

        state.mInputSize = cv::Size(width, height);

        state.calcRegions();
    }

    switch (state.mCurrentType) {
        case LRTBStripe: {
            cv::Mat lstripe(state.mLeftStripe.height, state.mLeftStripe.width, CV_8UC4);
            cv::Mat rstripe(state.mLeftStripe.height, state.mLeftStripe.width, CV_8UC4);
            cv::Mat tstripe(state.mTopStripe.height,  state.mTopStripe.width, CV_8UC4);
            cv::Mat bstripe(state.mTopStripe.height,  state.mTopStripe.width, CV_8UC4);

            state.mLRStripeConverter.convert(lstripe, state.mLeftStripe,   texture);
            state.mLRStripeConverter.convert(rstripe, state.mRightStripe,  texture);
            state.mTBStripeConverter.convert(tstripe, state.mTopStripe,    texture);
            state.mTBStripeConverter.convert(bstripe, state.mBottomStripe, texture);

            if (save_image) {
                save_image_with_random_name(lstripe);
                save_image_with_random_name(rstripe);
                save_image_with_random_name(tstripe);
                save_image_with_random_name(bstripe);
            }
        } break;
        case CardRect: {
            cv::Mat card_img(state.mCardRect.size(), CV_8UC4);
            state.mCardRectConverter.convert(card_img, state.mCardRect, texture);

            if (save_image)
                save_image_with_random_name(card_img);
        } break;
        case FullFrame: {
            cv::Mat whole_image(state.mInputSize, CV_8UC4);
            gCurrentConverter->convert(whole_image, cv::Rect({0,0}, state.mInputSize), texture);

            if (save_image)
                save_image_with_random_name(whole_image);
        }
    }

    /*
    //cv::Rect region { 0, 0, width, height };
    // currently unused
    // jfloatArray tm = (jfloatArray) env->GetObjectField(o, gCamTestLib_GlTexture_fields.textureTransform);
    // assert(env->GetArrayLength(tm) == 16);
    // std::memcpy(texture.transform.data(), env->GetFloatArrayElements(tm, nullptr), sizeof(float) * 16);

    using namespace std::chrono;

    // auto start_time = system_clock::now();

    bool res = gCurrentConverter->convert(texture_img, region, texture);

    // long elapsed = duration_cast<milliseconds>(system_clock::now() - start_time).count();
    // __android_log_print(ANDROID_LOG_DEBUG, TAG, "Time elapsed for convertion: %d", elapsed);

    if (save_image) {
        std::stringstream ss;
        ss << gImageSavePath << "/" << std::time(nullptr) << ".jpg";

        if (!cv::imwrite(ss.str(), texture_img))
            __android_log_print(ANDROID_LOG_WARN, TAG, "Failed to write texture with id %d to file %s.", texture_id, ss.str().c_str());
    }
    */
}
