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
/**
 * @brief Error handlers
 * @file errors.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_ERRORS_H
#define TESEO_HAL_ERRORS_H

#include <errno.h>

#define CHECK_ERROR(value, error_descriptor_func, ...) { \
if(value == -1) { error_descriptor_func(errno); } \
else            { ALOGV(__VA_ARGS__); } }

namespace stm {
namespace errors {

int read(int err);

int open(int err);

int close(int err);

int pipe(int err);

int write(int err);

} // namespace errors
} // namespace stm

#endif