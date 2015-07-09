LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_LDLIBS:= \
        -ljnigraphics \
        -llog \
        -landroid \
        -lGLESv2 # \
        # -lEGL

# We using OpenCV.mk script to configure this project  for OpenCV
OPENCVROOT := $(LOCAL_PATH)/../../tools/opencv
OPENCV_CAMERA_MODULES := off
OPENCV_INSTALL_MODULES := off
OPENCV_LIB_TYPE := STATIC
include ${OPENCVROOT}/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES := CamTest.cpp \
         FBOTextureConverter.cpp \
         RFBOTextureConverter.cpp \
         EGLITextureConverter.cpp
         # PBOTextureConverter.cpp \

LOCAL_MODULE := camtest

include $(BUILD_SHARED_LIBRARY)
