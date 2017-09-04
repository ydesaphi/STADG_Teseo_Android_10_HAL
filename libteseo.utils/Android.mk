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

# libteseo.utils module
include $(CLEAR_VARS)

LOCAL_MODULE := libteseo.utils
LOCAL_MODULE_OWNER := stm
LOCAL_MODULE_TAGS := optional

LOCAL_CPPFLAGS := $(TESEO_GLOBAL_CPPFLAGS)

LOCAL_SHARED_LIBRARIES := \
	libc                  \
	liblog                \
	libcutils             \
	libsysutils           \
	libhardware           \
	libcurl               \
	libteseo.vendor

LOCAL_SRC_FILES :=             \
	src/AbstractByteStream.cpp \
	src/ByteVector.cpp         \
	src/DebugOutputStream.cpp  \
	src/errors.cpp             \
	src/http.cpp               \
	src/NmeaStream.cpp         \
	src/Signal.cpp             \
	src/Thread.cpp             \
	src/Time.cpp               \
	src/UartByteStream.cpp     \
	src/utils.cpp              \
	src/Wakelock.cpp

LOCAL_COPY_HEADERS_TO:= teseo/utils/
LOCAL_COPY_HEADERS :=                       \
	include/teseo/utils/any.h               \
	include/teseo/utils/ByteVector.h        \
	include/teseo/utils/Channel.h           \
	include/teseo/utils/constraints.h       \
	include/teseo/utils/DebugOutputStream.h \
	include/teseo/utils/errors.h            \
	include/teseo/utils/http.h              \
	include/teseo/utils/IByteStream.h       \
	include/teseo/utils/IStream.h           \
	include/teseo/utils/NmeaStream.h        \
	include/teseo/utils/optional.h          \
	include/teseo/utils/result.h            \
	include/teseo/utils/Signal.h            \
	include/teseo/utils/Thread.h            \
	include/teseo/utils/Time.h              \
	include/teseo/utils/UartByteStream.h    \
	include/teseo/utils/utils.h             \
	include/teseo/utils/Wakelock.h

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

