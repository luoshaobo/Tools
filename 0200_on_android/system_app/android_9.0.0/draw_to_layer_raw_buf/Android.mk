LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=               \
        main.cpp               \

LOCAL_SHARED_LIBRARIES := \
	liblog libutils libbinder  \
          libgui libcutils libui
        
    # libstagefright libstagefright_foundation libmedia libmedia_native 

LOCAL_C_INCLUDES:= \
	frameworks/av/media/libstagefright \
	$(TOP)/frameworks/native/include/media/openmax

LOCAL_CFLAGS += -Wno-multichar

LOCAL_MODULE_TAGS := debug

LOCAL_MODULE:= draw_to_layer_raw_buf

include $(BUILD_EXECUTABLE)

