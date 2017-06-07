#ifndef TESEO_HAL_MODEL_CONSTELLATIONS
#define TESEO_HAL_MODEL_CONSTELLATIONS

#include <hardware/gps.h>

#include "TalkerId.h"

namespace stm {

enum class Constellation : uint8_t {
	Unknown = GNSS_CONSTELLATION_UNKNOWN,
	Gps     = GNSS_CONSTELLATION_GPS,
	Sbas    = GNSS_CONSTELLATION_SBAS,
	Glonass = GNSS_CONSTELLATION_GLONASS,
	Qzss    = GNSS_CONSTELLATION_QZSS,
	Beidou  = GNSS_CONSTELLATION_BEIDOU,
	Galileo = GNSS_CONSTELLATION_GALILEO
};

namespace ConstellationUtils {

GnssConstellationType toAndroidType(Constellation value);

Constellation convert(uint8_t value);

Constellation convert(TalkerId id);

}

} // namespace stm

#endif // TESEO_HAL_MODEL_CONSTELLATIONS