CommonCFlags = -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES
CommonCFlags += -Wall -Werror -Wunused -Wunreachable-code


# opengles_20_samples executable
# =========================================================

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += ${CommonCFlags}
LOCAL_CFLAGS += -Wno-error -Wno-format-security -Wno-unused-parameter -Wno-pointer-sign -Wno-return-type 

LOCAL_SRC_FILES:= \
    Common/esShader.c    \
    Common/esTransform.c \
    Common/esShapes.c    \
    Common/esUtil.c      \
    Common/log.cpp      \
    Chapter_2/Hello_Triangle/Hello_Triangle.c \
    Chapter_9/Simple_Texture2D/Simple_Texture2D.c \
    main.cpp
    
LOCAL_C_INCLUDES += \
    frameworks/base/cmds/opengles_20_samples/Common \

LOCAL_SHARED_LIBRARIES := \
    libEGL \
    libOpenSLES \
    libGLESv1_CM \
    libGLESv2 \
    libandroidfw \
    libbase \
    libbinder \
    libcutils \
    liblog \
    libutils \
    libui \
    libhwui \
    libgui \
    libtinyalsa \

LOCAL_MODULE:= opengles_20_samples

include $(BUILD_EXECUTABLE)

