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

# HAL module implemenation stored in
# hw/<GPS_HARDWARE_MODULE_ID>.<ro.hardware>.so

# gps hal module
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_MODULE := gps.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_OWNER := stm
LOCAL_MODULE_TAGS := optional

LOCAL_CPPFLAGS := $(TESEO_GLOBAL_CPPFLAGS)

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