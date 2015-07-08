LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += . \
        /home/daniel/work/AndroidSource/system/core/include/ \
        /home/daniel/work/AndroidSource/frameworks/native/include \
        /home/daniel/work/AndroidSource/hardware/libhardware/include/

LOCAL_LDLIBS:= \
        -ljnigraphics \
        -llog \
        -landroid \
        -lGLESv3 # \
        # -lEGL

#opencv
OPENCVROOT := $(LOCAL_PATH)/../../tools/opencv
OPENCV_CAMERA_MODULES := off
OPENCV_INSTALL_MODULES := off
OPENCV_LIB_TYPE := STATIC
include ${OPENCVROOT}/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES := CamTest.cpp \
         PBOTextureConverter.cpp \
         FBOTextureConverter.cpp \
         RFBOTextureConverter.cpp \
         EGLITextureConverter.cpp

LOCAL_MODULE := camtest

include $(BUILD_SHARED_LIBRARY)
