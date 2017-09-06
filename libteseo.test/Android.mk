#
# This file is part of Teseo Android HAL
#
# Copyright (c) 2016-2017, STMicroelectronics - All Rights Reserved
# Author(s): Baudouin Feildel <baudouin.feildel@st.com> for STMicroelectronics.
#
# License terms: Apache 2.0.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

# libteseo.test module
include $(CLEAR_VARS)

LOCAL_MODULE := libteseo.test
LOCAL_MODULE_OWNER := stm
LOCAL_MODULE_TAGS := eng

LOCAL_CPPFLAGS := -Wall -Wextra -std=c++1z -fexceptions -frtti

LOCAL_CPPFLAGS += -DDEBUG_NMEA_DECODER             # Debug the NMEA Decoder
LOCAL_CPPFLAGS += -DDISABLE_ALL_MESSAGE_DEBUGGING  # Disable all message debuggers (see messages.cpp)
LOCAL_CPPFLAGS += -DLOG_NDEBUG=0                   # Display ALOGV and ALOGD messages
#LOCAL_CPPFLAGS += -DDEBUG_HTTP_CLIENT              # Enable HTTP client debug messages
LOCAL_CPPFLAGS += -DSIGNAL_DEBUGGING               # Display signal debugging messages
#LOCAL_CPPFLAGS += -DENABLE_DEBUG_OUTPUT_STREAM     # Enable debug output stream

LOCAL_SHARED_LIBRARIES := \
	libc                  \
	liblog                \
	libcutils             \
	libsysutils           \
	libhardware           \
	libcurl               \
	libteseo.utils

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_SRC_FILES :=             \
	src/main.cpp               \
	src/utils/ByteVector.cpp

LOCAL_PRELINK_MODULE := false

include $(BUILD_EXECUTABLE)

