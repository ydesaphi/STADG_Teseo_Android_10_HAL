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
 * @brief Wrapper for the standard optional class
 * @file optional.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 * @details This file detect which version of optional is available, includes it and export it as
 * `std::optional` if needed.
 */
#ifndef TESEO_HAL_UTILS_OPTIONAL_H
#define TESEO_HAL_UTILS_OPTIONAL_H

#if __has_include(<optional>) && __cplusplus > 201402L
#include <optional>
#elif __has_include(<experimental/optional>) && __cplusplus >= 201402L
#include <experimental/optional>
namespace std {

template<typename T>
using optional = std::experimental::optional<T>;

}
#else
#error No optional support in stdlib
#endif

#endif