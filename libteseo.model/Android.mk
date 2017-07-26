LOCAL_PATH := $(call my-dir)

# libteseo.model module
include $(CLEAR_VARS)

LOCAL_MODULE := libteseo.model
LOCAL_MODULE_OWNER := stm
LOCAL_MODULE_TAGS := optional

LOCAL_CPPFLAGS := -Wall -Wextra -std=c++14 -fexceptions -frtti

LOCAL_SHARED_LIBRARIES := \
	libc                  \
	liblog                \
	libcutils             \
	libsysutils           \
	libhardware           \
	libteseo.utils

LOCAL_SRC_FILES :=              \
	src/Constellations.cpp      \
	src/Coordinate.cpp          \
	src/Location.cpp            \
	src/NmeaMessage.cpp         \
	src/SatInfo.cpp             \
	src/TalkerId.cpp            \
	src/Version.cpp

LOCAL_COPY_HEADERS_TO := teseo/model/
LOCAL_COPY_HEADERS :=                    \
	include/teseo/model/Constellations.h \
	include/teseo/model/Coordinate.h     \
	include/teseo/model/FixQuality.h     \
	include/teseo/model/Location.h       \
	include/teseo/model/Message.h        \
	include/teseo/model/NmeaMessage.h    \
	include/teseo/model/SatInfo.h        \
	include/teseo/model/TalkerId.h       \
	include/teseo/model/ValueContainer.h \
	include/teseo/model/Version.h

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

