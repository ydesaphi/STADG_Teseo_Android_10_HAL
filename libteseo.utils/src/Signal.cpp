/*
* This file is part of Teseo Android HAL
*
* Copyright (c) 2016-2017, STMicroelectronics - All Rights Reserved
* Author(s): Baudouin Feildel <baudouin.feildel@st.com> for STMicroelectronics.
*
* License terms: Apache 2.0.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include <teseo/utils/Signal.h>

#define LOG_TAG "teseo_hal_Signal"
#include <cutils/log.h>

#include <cstdarg>

namespace signal_debug {

void signal_log_verbose(const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	LOG_PRI_VA(ANDROID_LOG_VERBOSE, LOG_TAG, fmt, args);
	va_end(args);
}

void signal_log_debug(const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	LOG_PRI_VA(ANDROID_LOG_DEBUG, LOG_TAG, fmt, args);
	va_end(args);
}

void signal_log_info(const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	LOG_PRI_VA(ANDROID_LOG_INFO, LOG_TAG, fmt, args);
	va_end(args);
}

void signal_log_warning(const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	LOG_PRI_VA(ANDROID_LOG_WARN, LOG_TAG, fmt, args);
	va_end(args);
}

void signal_log_error(const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	LOG_PRI_VA(ANDROID_LOG_ERROR, LOG_TAG, fmt, args);
	va_end(args);
}

} // namespace signal_debug
