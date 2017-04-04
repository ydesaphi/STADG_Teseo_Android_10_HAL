#ifndef TESEO_HAL_UTILS_TIME_H
#define TESEO_HAL_UTILS_TIME_H

#include <hardware/gps.h>
#include "ByteVector.h"

namespace stm {
namespace utils {

GpsUtcTime parseTimestamp(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end);

GpsUtcTime parseTimestamp(const ByteVector & vec);

int injectTime(GpsUtcTime time, int64_t timeReference, int uncertainty);

} // namespace utils
} // namespace stm

#endif // TESEO_HAL_UTILS_TIME_H