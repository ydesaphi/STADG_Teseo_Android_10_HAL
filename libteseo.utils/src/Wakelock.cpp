#include <teseo/utils/Wakelock.h>

namespace stm {
namespace utils {
Signal<void> Wakelock::acquire("Wakelock::acquire");
Signal<void> Wakelock::release("Wakelock::release");
}
}