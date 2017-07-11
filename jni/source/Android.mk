LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := GFramework
LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS=1
LOCAL_CFLAGS += -Wno-psabi
MY_RELATIVE_PATH  := $(LOCAL_PATH)#$(subst $(MY_BASE_JNI_PATH)/,,$(LOCAL_PATH))
LOCAL_SRC_FILES   := $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GVideoPlayer/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GEventManager/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GDataConverter/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GCommandManager/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GDeviceDetecter/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GAudioManager/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GAudioManager/GAudioEngine/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GAudioManager/GAudioEngine/GAudioIOEngine/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GAudioManager/GAudioEngine/GAudioRecorder/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GAudioManager/GAudioEngine/GAudioPlayer/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GAudioManager/GAudioHornUpdate/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GAudioManager/GAudioSaveFile/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GAudioManager/GAudioUtility/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GAudioManager/GAudioFilePlayer/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GAudioManager/GAudioStreamPlayer/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GAudioManager/GAudioStreamRecorder/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GAudioManager/GAudioDeleteFile/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GAudioManager/GAudioFileRecorder/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GCameraManager/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GVideoRecorder/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GVideoRecorder/GVideoRecordBuffer/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GVideoRecorder/GVideoRecordUtility/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GTasksKiller/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GScreenshotManager/*.c*))
$(warning LOCAL_SRC_FILES is $(LOCAL_SRC_FILES))

#LOCAL_LDLIBS := -llog -lc
LOCAL_LDLIBS := -lm -llog

LOCAL_C_INCLUDES := \
	$(MY_BASE_JNI_PATH)/framework/live555/BasicUsageEnvironment/include/ \
	$(MY_BASE_JNI_PATH)/framework/live555/groupsock/include/ \
	$(MY_BASE_JNI_PATH)/framework/live555/liveMedia/include/ \
	$(MY_BASE_JNI_PATH)/framework/live555/UsageEnvironment/include/ \
	$(MY_BASE_JNI_PATH)/framework/ffmpeg/ \
	$(MY_BASE_JNI_PATH)/framework/ffmpeg/libavcodec/ \
	$(MY_BASE_JNI_PATH)/framework/ffmpeg/libavdevice/ \
	$(MY_BASE_JNI_PATH)/framework/ffmpeg/libavfilter/ \
	$(MY_BASE_JNI_PATH)/framework/ffmpeg/libavformat/ \
	$(MY_BASE_JNI_PATH)/framework/ffmpeg/libavresample/ \
	$(MY_BASE_JNI_PATH)/framework/ffmpeg/libavutil/ \
	$(MY_BASE_JNI_PATH)/framework/ffmpeg/libpostproc/ \
	$(MY_BASE_JNI_PATH)/framework/ffmpeg/libswresample/ \
	$(MY_BASE_JNI_PATH)/framework/ffmpeg/libswscale/ \
	$(MY_BASE_JNI_PATH)/framework/opengles/include/

include $(BUILD_STATIC_LIBRARY)