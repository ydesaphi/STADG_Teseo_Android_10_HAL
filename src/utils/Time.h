/**
 * @brief Clock time utilities
 * @file Time.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_UTILS_TIME_H
#define TESEO_HAL_UTILS_TIME_H

#include <hardware/gps.h>
#include "ByteVector.h"

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
GpsUtcTime parseTimestamp(
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
GpsUtcTime parseTimestamp(const ByteVector & vec);

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

} // namespace utils
} // namespace stm

#endif // TESEO_HAL_UTILS_TIME_H