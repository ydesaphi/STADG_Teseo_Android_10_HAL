LOCAL_PATH := $(call my-dir)

# libteseo module
include $(CLEAR_VARS)

LOCAL_MODULE := libteseo
LOCAL_MODULE_OWNER := stm
LOCAL_MODULE_TAGS := optional

LOCAL_CPPFLAGS := -Wall -Wextra -std=c++14 -fexceptions -frtti

LOCAL_SHARED_LIBRARIES := \
	libc                  \
	liblog                \
	libcutils             \
	libsysutils           \
	libhardware           \
	libteseo.core         \
	libteseo.device       \
	libteseo.protocol     \
	libteseo.model        \
	libteseo.config       \
	libteseo.utils

ifeq ($(TESEO_STAGPS_ENABLED),true)
	LOCAL_CPPFLAGS += -DSTAGPS_ENABLED
	LOCAL_SHARED_LIBRARIES += libstagps
endif

LOCAL_SRC_FILES := src/teseo.cpp

LOCAL_COPY_HEADERS_TO:= teseo/
LOCAL_COPY_HEADERS := include/teseo/teseo.h

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

