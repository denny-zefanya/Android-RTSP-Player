LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := GMediaEngine
LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS=1
LOCAL_CFLAGS += -Wno-psabi

MY_RELATIVE_PATH  := $(LOCAL_PATH)#$(subst $(MY_BASE_JNI_PATH)/,,$(LOCAL_PATH))
LOCAL_SRC_FILES   := $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/*.c*))

#GMAudio
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMAudio/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMAudio/AudioConverter/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMAudio/AudioFileFrame/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMAudio/AudioFileSession/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMAudio/AudioRecordFrame/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMAudio/AudioRecordSession/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMAudio/AudioPlayer/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMAudio/AudioRecorder/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMAudio/Controller/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMAudio/PCMMediaSink/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMAudio/RTSPClient/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMAudio/RTSPConfiger/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMAudio/RTSPServer/*.c*))

#GMVideo
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMVideo/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMVideo/Controller/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMVideo/H264Decoder/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMVideo/H264MediaSink/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMVideo/RTSPClient/*.c*))
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMVideo/RTSPConfiger/*.c*))

#GMUtility
LOCAL_SRC_FILES   += $(subst $(MY_RELATIVE_PATH)/,,$(wildcard $(MY_RELATIVE_PATH)/GMUtility/*.c*))

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

LOCAL_LDLIBS := -lm -llog

#include $(PREBUILT_STATIC_LIBRARY)	
include $(BUILD_STATIC_LIBRARY)