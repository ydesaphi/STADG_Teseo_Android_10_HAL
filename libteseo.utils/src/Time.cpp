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
 * @file Time.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/utils/Time.h>

#define LOG_TAG "teseo_hal_utils_Time"
#include <cutils/log.h>
#include <chrono>
#include <ctime>
#include <cstring>
#include <cinttypes>
namespace stm {
namespace utils {

using namespace std;
using namespace std::chrono;

static struct {
	GpsUtcTime time;
	int uncertainty;
	time_point<system_clock> timePoint;
} utcNow;

static time_point<system_clock> utcTodayOffset;


int injectTime(GpsUtcTime time, int64_t timeReference, int uncertainty)
{
	ALOGI("Inject time: %" PRId64 ", reference: %" PRId64 ", uncertainty: %d -- now: %lld",
		time, timeReference, uncertainty,
		duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());

	utcNow.time = time;
	utcNow.uncertainty = uncertainty;
	utcNow.timePoint = time_point<system_clock>(milliseconds(utcNow.time));

	ALOGI("Date time: %s", time2string(utcNow.timePoint).c_str());

	time_t timestamp = system_clock::to_time_t(utcNow.timePoint);
	tm timeInfo;
	memcpy(&timeInfo, gmtime(&timestamp), sizeof(tm));

	timeInfo.tm_sec = 0;
	timeInfo.tm_min = 0;
	timeInfo.tm_hour = 0;

	utcTodayOffset = system_clock::from_time_t(mktime(&timeInfo));

	return 0;
}

GpsUtcTime systemNow()
{
	return static_cast<GpsUtcTime>(
		duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
}

std::optional<GpsUtcTime> parseTimestamp(const ByteVector & vec)
{
	return parseTimestamp(vec.cbegin(), vec.cend());
}

std::string time2string(GpsUtcTime tp)
{
	return time2string(time_point<system_clock>(milliseconds(tp)));
}

GpsUtcTime utc_timestamp_to_gps_timestamp(GpsUtcTime tp)
{
	// Not a magic number, juste the timestamp of GPS Time origin
	// GPS Time origin is : 1980-01-06T00:00:00.000
	return tp - 315964800000;
}

// Timestamp expected format : hhmmss.msec
//                             0123456789.
// We ignore the '.' before msec field as it make the parse int function crash
constexpr int PARSER_HOUR_SIZE = 2, PARSER_HOUR_OFFSET = 0;
constexpr int PARSER_MIN_SIZE  = 2, PARSER_MIN_OFFSET  = 2;
constexpr int PARSER_SEC_SIZE  = 2, PARSER_SEC_OFFSET  = 4;
constexpr int PARSER_MSEC_SIZE = 3, PARSER_MSEC_OFFSET = 7;

std::optional<GpsUtcTime> parseTimestamp(
	const ByteVector::const_iterator & begin,
	const ByteVector::const_iterator & end)
{
	std::optional<int> hour, min, sec, msec;

	hour = byteVectorParse<int>(
		begin + PARSER_HOUR_OFFSET,
		begin + PARSER_HOUR_OFFSET + PARSER_HOUR_SIZE);

	min = byteVectorParse<int>(
		begin + PARSER_MIN_OFFSET,
		begin + PARSER_MIN_OFFSET + PARSER_MIN_SIZE);

	sec = byteVectorParse<int>(
		begin + PARSER_SEC_OFFSET,
		begin + PARSER_SEC_OFFSET + PARSER_SEC_SIZE);

	msec = byteVectorParse<int>(
		begin + PARSER_MSEC_OFFSET,
		begin + PARSER_MSEC_OFFSET + PARSER_MSEC_SIZE);

	if(end != begin + PARSER_MSEC_OFFSET + PARSER_MSEC_SIZE)
		ALOGW("Trailing data after timestamp: '%s'", bytesToString(begin, end).c_str());

	if(hour && min && sec && msec)
		return *msec + *sec * 1000 + *min * 60000 + *hour * 3600000 +
			duration_cast<milliseconds>(utcTodayOffset.time_since_epoch()).count();
	else
		return {};
}

} // namespace utils
} // namespace stm