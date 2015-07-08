#pragma once

#include <android/log.h>

static const char* TAG = "camtest-native-tag";

#define ASSERT(cond, ...) \
    do { \
        if (!(cond)) \
            __android_log_assert(#cond, TAG, ##__VA_ARGS__); \
    } while (0);

#define LOGD(...) \
    __android_log_print(ANDROID_LOG_DEBUG, TAG, ##__VA_ARGS__);

#define LOGI(...) \
    __android_log_print(ANDROID_LOG_INFO, TAG, ##__VA_ARGS__);

#define LOGV(...) \
    __android_log_print(ANDROID_LOG_VERBOSE, TAG, ##__VA_ARGS__);

#define LOGW(...) \
    __android_log_print(ANDROID_LOG_WARN, TAG, ##__VA_ARGS__);
