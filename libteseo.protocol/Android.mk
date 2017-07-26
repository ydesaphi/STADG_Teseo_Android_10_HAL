LOCAL_PATH := $(call my-dir)

# libteseo.protocol module
include $(CLEAR_VARS)

LOCAL_MODULE := libteseo.protocol
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
	libteseo.model        \
	libteseo.device       \
	libteseo.vendor

LOCAL_SRC_FILES :=              \
	src/nmea/messages.cpp       \
	src/AbstractDecoder.cpp     \
	src/NmeaDecoder.cpp         \
	src/NmeaEncoder.cpp

LOCAL_COPY_HEADERS_TO:= teseo/protocol/
LOCAL_COPY_HEADERS :=                        \
	include/teseo/protocol/AbstractDecoder.h \
	include/teseo/protocol/IEncoder.h        \
	include/teseo/protocol/NmeaDecoder.h     \
	include/teseo/protocol/NmeaEncoder.h

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

