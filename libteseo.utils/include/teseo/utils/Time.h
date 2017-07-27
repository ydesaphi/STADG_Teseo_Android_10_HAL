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