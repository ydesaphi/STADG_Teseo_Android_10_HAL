LOCAL_PATH := $(call my-dir)

# libteseo.core module
include $(CLEAR_VARS)

LOCAL_MODULE := libteseo.core
LOCAL_MODULE_OWNER := stm
LOCAL_MODULE_TAGS := optional

LOCAL_CPPFLAGS := -Wall -Wextra -std=c++14 -fexceptions -frtti

LOCAL_SHARED_LIBRARIES := \
	libc                  \
	liblog                \
	libcutils             \
	libsysutils           \
	libhardware           \
	libteseo.vendor       \
	libteseo.utils        \
	libteseo.config       \
	libteseo.model        \
	libteseo.device       \
	libteseo.protocol

ifeq ($(TESEO_STAGPS_ENABLED),true)
	LOCAL_CPPFLAGS += -DSTAGPS_ENABLED
	LOCAL_SHARED_LIBRARIES += libstagps
endif

LOCAL_SRC_FILES :=                  \
	src/HalManager.cpp              \
	src/LocServiceProxy.cpp

LOCAL_COPY_HEADERS_TO:= teseo/
LOCAL_COPY_HEADERS :=               \
	include/teseo/HalManager.h      \
	include/teseo/LocServiceProxy.h

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

