LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    	:= liveMedia

MY_RELATIVE_PATH  := $(LOCAL_PATH)#$(subst $(MY_BASE_JNI_PATH)/,,$(LOCAL_PATH))
MY_RELATIVE_FILES := $(wildcard $(MY_RELATIVE_PATH)/source/*.c*)
LOCAL_SRC_FILES   := $(subst $(MY_RELATIVE_PATH)/,,$(MY_RELATIVE_FILES))

LOCAL_LDLIBS 		:= -lm -llog
LOCAL_C_INCLUDES 	:= \
	$(MY_BASE_JNI_PATH)/framework/live555/BasicUsageEnvironment/include \
	$(MY_BASE_JNI_PATH)/framework/live555/groupsock/include \
	$(MY_BASE_JNI_PATH)/framework/live555/$(LOCAL_MODULE)/include \
	$(MY_BASE_JNI_PATH)/framework/live555/UsageEnvironment/include
	
LOCAL_CFLAGS 		:= -DNULL=0 -DSOCKLEN_T=socklen_t
LOCAL_CPPFLAGS 		:= -DBSD=1 -DXLOCALE_NOT_USED
	
#include $(PREBUILT_STATIC_LIBRARY)	
include $(BUILD_STATIC_LIBRARY)