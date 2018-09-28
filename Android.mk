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

# Global compiler configuration
TESEO_GLOBAL_CPPFLAGS := -Wall -Wextra -std=c++1z -fexceptions -frtti

# Extra flags
# These flags can be commented or not to enable or disable some features of the HAL
#TESEO_GLOBAL_CPPFLAGS += -DDEBUG_NMEA_DECODER             # Debug the NMEA Decoder
TESEO_GLOBAL_CPPFLAGS += -DDEBUG_NMEA_LOG_OUTPUT         # Output the NMEA messages
#TESEO_GLOBAL_CPPFLAGS += -DDISABLE_ALL_MESSAGE_DEBUGGING  # Disable all message debuggers (see messages.cpp)
TESEO_GLOBAL_CPPFLAGS += -DLOG_NDEBUG=0                   # Display ALOGV and ALOGD messages
#TESEO_GLBOAL_CPPFLAGS += -DDEBUG_HTTP_CLIENT              # Enable HTTP client debug messages
#TESEO_GLOBAL_CPPFLAGS += -DSIGNAL_DEBUGGING               # Display signal debugging messages
#TESEO_GLOBAL_CPPFLAGS += -DENABLE_DEBUG_OUTPUT_STREAM     # Enable debug output stream

# Auto-detect optional modules
ifeq ($(shell test -d $(LOCAL_PATH)/libstagps && echo true),true)
TESEO_STAGPS_ENABLED := true
else
TESEO_STAGPS_ENABLED := false
endif

ifeq ($(shell test -d $(LOCAL_PATH)/libteseo.straw && echo true),true)
TESEO_STRAW_ENABLED := true
else
TESEO_STRAW_ENABLED := false
endif
ifeq ($(shell test -d $(LOCAL_PATH)/libteseo.agnss && echo true),true)
TESEO_AGPS_ENABLED := true
TESEO_SUPL_ENABLED := false
else
TESEO_AGPS_ENABLED := false
TESEO_SUPL_ENABLED := false
endif

include $(call all-subdir-makefiles)
