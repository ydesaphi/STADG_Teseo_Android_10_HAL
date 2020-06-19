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
 * @brief Wrapper for the standard any class
 * @file any.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 * @details This file detect which version of any is available, includes it and export it as
 * `std::any` if needed.
 */
#ifndef TESEO_HAL_UTILS_ANY_H
#define TESEO_HAL_UTILS_ANY_H

#if __has_include(<optional>) && __cplusplus > 201402L
#include <any>
#elif __has_include(<experimental/optional>) && __cplusplus >= 201402L
#include <experimental/any>
namespace std {

using any = std::experimental::any;

template<class ValueType>
ValueType any_cast(const any & a)
{
	return std::experimental::any_cast<ValueType>(a);
}

template<class ValueType>
ValueType any_cast(any & a)
{
	return std::experimental::any_cast<ValueType>(a);
}

template<class ValueType>
ValueType any_cast(any && a)
{
	return std::experimental::any_cast<ValueType>(a);
}

template<class ValueType>
ValueType any_cast(const any * a) noexcept
{
	return std::experimental::any_cast<ValueType>(a);
}

template<class ValueType>
ValueType any_cast(any * a) noexcept
{
	return std::experimental::any_cast<ValueType>(a);
}

}
#else
#error No any support in stdlib
#endif

#endif // TESEO_HAL_UTILS_ANY_H