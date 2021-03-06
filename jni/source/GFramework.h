/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_greenbamboo_prescholleducation_MediaFramework_GMediaFramework */

#ifndef _Included_com_greenbamboo_prescholleducation_MediaFramework_GMediaFramework
#define _Included_com_greenbamboo_prescholleducation_MediaFramework_GMediaFramework
#ifdef __cplusplus
extern "C" {
#endif

#define DO_DEBUG										1
#define EVENT_CODE_555(x)						( (x) << 30 )
#define EVENT_CODE_TYPE(x)					( (x) << 24 )
#define EVENT_CODE_ACTION(x)				( (x) << 16 )
#define EVENT_CODE_VALUE(x)				( (x) << 8  )
#define EVENT_CODE_TYPE_VIDEO			EVENT_CODE_TYPE( 1 )
#define EVENT_CODE_TYPE_AUDIO			EVENT_CODE_TYPE( 0 )
#define EVENT_CODE_TYPE_555				EVENT_CODE_555( 1 )



#define EVENT_CODE_ACTION_START			EVENT_CODE_ACTION( 1 )
#define EVENT_CODE_ACTION_STOP			EVENT_CODE_ACTION( 0 )
#define EVENT_CODE_VALUE_SUCCESS		EVENT_CODE_VALUE( 1 )
#define EVENT_CODE_VALUE_FAILED			EVENT_CODE_VALUE( 0 )
#define EVENT_CODE_FIRST_FRAME(w, h)	( ( 1 << 31 ) | ( ( ( (w) << 17 ) >> 17 ) << 15 ) | ( ( (h) << 17 ) >> 17 ) )

#if	DO_DEBUG
#include <android/log.h>
#define printf(...) __android_log_print( ANDROID_LOG_VERBOSE, "GMediaFramework", __VA_ARGS__ );
#else
#define printf(...) void(0)
#endif

#ifndef N_ELEMENTS
# define N_ELEMENTS(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

#ifdef __cplusplus
}
#endif
#endif
