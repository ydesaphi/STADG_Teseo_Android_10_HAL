LOCAL_PATH := $(call my-dir)

# libteseo.utils module
include $(CLEAR_VARS)

LOCAL_MODULE := libteseo.utils
LOCAL_MODULE_OWNER := stm
LOCAL_MODULE_TAGS := optional

LOCAL_CPPFLAGS := -Wall -Wextra -std=c++14 -fexceptions -frtti

LOCAL_SHARED_LIBRARIES := \
	libc                  \
	liblog                \
	libcutils             \
	libsysutils           \
	libhardware           \
	libteseo.vendor

LOCAL_SRC_FILES :=         \
	src/ByteVector.cpp     \
	src/errors.cpp         \
	src/NmeaStream.cpp     \
	src/Thread.cpp         \
	src/Time.cpp           \
	src/UartByteStream.cpp \
	src/Wakelock.cpp

LOCAL_COPY_HEADERS_TO:= teseo/utils/
LOCAL_COPY_HEADERS :=                    \
	include/teseo/utils/ByteVector.h     \
	include/teseo/utils/Channel.h        \
	include/teseo/utils/constraints.h    \
	include/teseo/utils/errors.h         \
	include/teseo/utils/IByteStream.h    \
	include/teseo/utils/IStream.h        \
	include/teseo/utils/log.h            \
	include/teseo/utils/NmeaStream.h     \
	include/teseo/utils/optional.h       \
	include/teseo/utils/result.h         \
	include/teseo/utils/Signal.h         \
	include/teseo/utils/Thread.h         \
	include/teseo/utils/Time.h           \
	include/teseo/utils/UartByteStream.h \
	include/teseo/utils/utils.h          \
	include/teseo/utils/Wakelock.h

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

