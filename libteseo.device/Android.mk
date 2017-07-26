LOCAL_PATH := $(call my-dir)

# libteseo.device module
include $(CLEAR_VARS)

LOCAL_MODULE := libteseo.device
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
	libteseo.config       \
	libteseo.model

LOCAL_SRC_FILES :=         \
	src/AbstractDevice.cpp \
	src/NmeaDevice.cpp

LOCAL_COPY_HEADERS_TO:= teseo/device/
LOCAL_COPY_HEADERS :=                     \
	include/teseo/device/AbstractDevice.h \
	include/teseo/device/NmeaDevice.h

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

