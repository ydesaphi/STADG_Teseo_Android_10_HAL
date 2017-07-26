LOCAL_PATH := $(call my-dir)

# libteseo.config module
include $(CLEAR_VARS)

LOCAL_MODULE := libteseo.config
LOCAL_MODULE_OWNER := stm
LOCAL_MODULE_TAGS := optional

LOCAL_CPPFLAGS := -Wall -Wextra -std=c++14 -fexceptions -frtti

LOCAL_SHARED_LIBRARIES := \
	libc                  \
	liblog                \
	libcutils             \
	libsysutils           \
	libhardware           \
	libteseo.utils        \
	libteseo.vendor

LOCAL_SRC_FILES := src/config.cpp

LOCAL_COPY_HEADERS_TO:= teseo/config/
LOCAL_COPY_HEADERS := include/teseo/config/config.h

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

