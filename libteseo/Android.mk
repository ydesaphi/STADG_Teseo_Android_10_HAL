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

# libteseo module
include $(CLEAR_VARS)

LOCAL_MODULE := libteseo
LOCAL_MODULE_OWNER := stm
LOCAL_MODULE_TAGS := optional

LOCAL_CPPFLAGS := $(TESEO_GLOBAL_CPPFLAGS)

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

ifeq ($(TESEO_AGPS_ENABLED),true)
	LOCAL_CPPFLAGS += -DAGPS_ENABLED
	LOCAL_SHARED_LIBRARIES += libteseo.agnss
endif

ifeq ($(TESEO_SUPL_ENABLED),true)
	LOCAL_CPPFLAGS += -DSUPL_ENABLED
endif

LOCAL_SRC_FILES := src/teseo.cpp

LOCAL_COPY_HEADERS_TO:= teseo/
LOCAL_COPY_HEADERS := include/teseo/teseo.h

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

