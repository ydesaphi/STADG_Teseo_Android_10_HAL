LOCAL_PATH := $(call my-dir)

# libteseo.vendor module
include $(CLEAR_VARS)

LOCAL_MODULE := libteseo.vendor
LOCAL_MODULE_OWNER := stm
LOCAL_MODULE_TAGS := optional

LOCAL_CPPFLAGS := -Wall -Wextra -std=c++14 -fexceptions -frtti

LOCAL_SHARED_LIBRARIES := \
	libc                  \
	liblog                \
	libcutils             \
	libsysutils           \
	libhardware

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

