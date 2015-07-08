#pragma once

#include <string>
#include <jni.h>


///////////////////////////////////////////////////////////
//////////////////////// TOOLS ////////////////////////////
unsigned char* as_unsigned_char_array(JNIEnv* env, jbyteArray array) {
    int len = env->GetArrayLength(array);
    unsigned char* buf = new unsigned char[len];
    env->GetByteArrayRegion (array, 0, len, reinterpret_cast<jbyte*>(buf));
    return buf;
}

std::string as_std_string(JNIEnv* env, jstring jstr) {
  const char* s = env->GetStringUTFChars(jstr, NULL);

  std::string res = s;

  env->ReleaseStringUTFChars(jstr, s);

  return res;
}
