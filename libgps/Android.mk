LOCAL_PATH := $(call my-dir)

# HAL module implemenation stored in
# hw/<GPS_HARDWARE_MODULE_ID>.<ro.hardware>.so

# gps hal module
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_MODULE := gps.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_OWNER := stm
LOCAL_MODULE_TAGS := optional

LOCAL_CPPFLAGS := -Wall -Wextra -std=c++14

LOCAL_SHARED_LIBRARIES := \
	libc                  \
	liblog                \
	libcutils             \
	libsysutils           \
	libhardware           \
	libteseo.vendor       \
	libteseo              \
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

LOCAL_SRC_FILES :=    \
	gps.cpp

include $(BUILD_SHARED_LIBRARY)