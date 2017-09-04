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
 * @brief Clock time utilities
 * @file Time.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_UTILS_TIME_H
#define TESEO_HAL_UTILS_TIME_H

#include <chrono>
#include <iomanip>
#include <hardware/gps.h>
#include <sstream>
#include "ByteVector.h"
#include "optional.h"

namespace stm {
namespace utils {

/**
 * @brief      Convert a byte vector to a timestamp
 * 
 * @details    The timestamp format is 'hhmmss.msec'. To output a complete timestamp we used the
 * current UTC date injected by the platform.
 *
 * @param[in]  begin  Byte vector beginning
 * @param[in]  end    Byte vector end (excluded from analysis)
 *
 * @return     The parsed timestamp
 */
std::optional<GpsUtcTime> parseTimestamp(
	const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end);

/**
 * @brief      Convert a byte vector to a timestamp
 * 
 * @details    The timestamp format is 'hhmmss.msec'. To output a complete timestamp we used the
 * current UTC date injected by the platform.
 *
 * @param[in]  vec    Byte vector to parse
 *
 * @return     The parsed timestamp
 */
std::optional<GpsUtcTime> parseTimestamp(const ByteVector & vec);

/**
 * @brief      Save the UTC time into the HAL memory
 *
 * @param[in]  time           The time
 * @param[in]  timeReference  The time reference
 * @param[in]  uncertainty    The uncertainty
 *
 * @return     always 0
 */
int injectTime(GpsUtcTime time, int64_t timeReference, int uncertainty);

/**
 * @brief      Get the current system UTC time
 *
 * @return     The current system UTC time
 */
GpsUtcTime systemNow();

template<class ClockT>
std::string time2string(const std::chrono::time_point<ClockT> & tp)
{
	auto tt = ClockT::to_time_t(tp);
	std::ostringstream oss;

	oss << std::put_time(std::gmtime(&tt), "%c %Z");

	return oss.str();
}

std::string time2string(GpsUtcTime tp);

GpsUtcTime utc_timestamp_to_gps_timestamp(GpsUtcTime tp);

} // namespace utils
} // namespace stm

template<class CharT, class Traits, class ClockT>
std::basic_ostream<CharT, Traits> & operator << (
	std::basic_ostream<CharT, Traits> & stream, const std::chrono::time_point<ClockT> & tp)
{
	auto tt = ClockT::to_time_t(tp);
	return stream << std::put_time(std::gmtime(&tt), "%c %Z");
}

#endif // TESEO_HAL_UTILS_TIME_H