/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_walletone_camlib_CamTestLib */

#ifndef _Included_com_walletone_camlib_CamTestLib
#define _Included_com_walletone_camlib_CamTestLib
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_walletone_camlib_CamTestLib
 * Method:    nativeSetTextureSaveMethod
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_walletone_camlib_CamTestLib_nativeSetTextureSaveMethod
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_walletone_camlib_CamTestLib
 * Method:    nativeSetConvertMode
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_walletone_camlib_CamTestLib_nativeSetConvertMode
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_walletone_camlib_CamTestLib
 * Method:    nativeSetImageSavePath
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_walletone_camlib_CamTestLib_nativeSetImageSavePath
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_walletone_camlib_CamTestLib
 * Method:    nativeSaveTexture
 * Signature: (Lcom/walletone/camlib/CamTestLib/GlTexture;IIZ)V
 */
JNIEXPORT void JNICALL Java_com_walletone_camlib_CamTestLib_nativeSaveTexture
  (JNIEnv *, jclass, jobject, jint, jint, jboolean);

#ifdef __cplusplus
}
#endif
#endif
