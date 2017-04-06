/**
 * @brief Clock time utilities
 * @file Time.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include "Time.h"

#define LOG_TAG "teseo_hal_utils_Time"
#include <cutils/log.h>
#include <chrono>
#include <ctime>
#include <cstring>

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
	ALOGI("Inject time: %lld, reference: %lld, uncertainty: %d -- now: %lld", time, timeReference, uncertainty,
		duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
	utcNow.time = time;
	utcNow.uncertainty = uncertainty;
	utcNow.timePoint = time_point<system_clock>(milliseconds(utcNow.time));

	time_t timestamp = system_clock::to_time_t(utcNow.timePoint);
	tm timeInfo;
	memcpy(&timeInfo, gmtime(&timestamp), sizeof(tm));

	timeInfo.tm_sec = 0;
	timeInfo.tm_min = 0;
	timeInfo.tm_hour = 0;

	utcTodayOffset = system_clock::from_time_t(mktime(&timeInfo));

	return 0;
}

GpsUtcTime parseTimestamp(const ByteVector & vec)
{
	return parseTimestamp(vec.cbegin(), vec.cend());
}

GpsUtcTime parseTimestamp(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
{
	// Timestamp expected format : hhmmss.msec
	int hour = byteVectorParseInt(begin, begin + 2);
	int min = byteVectorParseInt(begin + 2, begin + 4);
	int sec = byteVectorParseInt(begin + 4, begin + 6);
	// We ignore the '.' before msec field as it make the parse int function crash
	int msec = byteVectorParseInt(begin + 7, end);

	return msec + sec * 1000 + min * 60000 + hour * 3600000 +
		duration_cast<milliseconds>(utcTodayOffset.time_since_epoch()).count();
}

} // namespace utils
} // namespace stm