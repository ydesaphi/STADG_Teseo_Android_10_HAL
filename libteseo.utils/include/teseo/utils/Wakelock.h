#ifndef TESEO_HAL_UTILS_WAKELOCK_H
#define TESEO_HAL_UTILS_WAKELOCK_H

#include "Signal.h"

namespace stm {
namespace utils {

class Wakelock {
public:
	static Signal<void> acquire;
	static Signal<void> release;
};

} // namespace utils
} // namespace stm

#endif // TESEO_HAL_UTILS_WAKELOCK_H