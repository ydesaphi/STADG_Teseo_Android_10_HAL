LOCAL_PATH := $(call my-dir)

# HAL module implemenation stored in
# hw/<GPS_HARDWARE_MODULE_ID>.<ro.hardware>.so

# libteseo module
include $(CLEAR_VARS)

LOCAL_MODULE := libteseo
LOCAL_MODULE_OWNER := stm
LOCAL_MODULE_TAGS := optional

LOCAL_CPPFLAGS := -Wall -Wextra -std=c++14 -fexceptions

LOCAL_SHARED_LIBRARIES := \
	libc                  \
	liblog                \
	libcutils             \
	libsysutils           \
	libhardware

LOCAL_SRC_FILES :=            \
	HalManager.cpp            \
	errors.cpp                \
	stream/NmeaStream.cpp     \
	decoder/IDecoder.cpp      \
	decoder/NmeaDecoder.cpp   \
	utils/ByteVector.cpp      \
	utils/Time.cpp            \
	model/NmeaMessage.cpp     \
	model/TalkerId.cpp        \
	model/Location.cpp        \
	model/Coordinate.cpp      \
	LocServiceProxy.cpp       \
	decoder/nmea/messages.cpp \
	device/AbstractDevice.cpp \
	device/NmeaDevice.cpp     \
	thread/Thread.cpp

#LOCAL_COPY_HEADERS_TO:= libteseo/
#LOCAL_COPY_HEADERS := \
#	GpsInterface.h

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

# gps hal module
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_MODULE := gps.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_OWNER := stm
LOCAL_MODULE_TAGS := optional

LOCAL_CPPFLAGS := -Wall -Wextra -std=c++14 -fexceptions

LOCAL_SHARED_LIBRARIES := \
	libteseo              \
	liblog                \
	libcutils             \
	libhardware

LOCAL_SRC_FILES :=    \
	gps.cpp

LOCAL_COPY_FILES += \
    $(LOCAL_PATH)/../etc/gps.conf:system/etc/gps.conf

include $(BUILD_SHARED_LIBRARY)
