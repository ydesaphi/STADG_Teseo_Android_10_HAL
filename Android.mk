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
TESEO_GLOBAL_CPP_FLAGS += -DDEBUG_NMEA_DECODER

# Auto-detect optional modules
ifeq ($(shell test -d $(LOCAL_PATH)/libstagps && echo true),true)
TESEO_STAGPS_ENABLED := true
else
TESEO_STAGPS_ENABLED := false
endif

include $(call all-subdir-makefiles)
