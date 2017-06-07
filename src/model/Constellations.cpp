#include "Constellations.h"

namespace stm {
namespace ConstellationUtils {

GnssConstellationType toAndroidType(Constellation value)
{
	return static_cast<GnssConstellationType>(value);
}

Constellation convert(uint8_t value)
{
	if(value > static_cast<uint8_t>(Constellation::Galileo))
		return Constellation::Unknown;

	return static_cast<Constellation>(value);
}

Constellation convert(TalkerId id)
{
	switch(id)
	{
		case TalkerId::GP: return Constellation::Gps;
		case TalkerId::GA: return Constellation::Galileo;
		case TalkerId::GL: return Constellation::Glonass;
		case TalkerId::GN: return Constellation::Unknown;
		case TalkerId::GB: return Constellation::Beidou;
		case TalkerId::BD: return Constellation::Beidou;
		case TalkerId::QZ: return Constellation::Qzss;
		default:           return Constellation::Unknown;
	}
}

} // namespace ConstellationUtils
} // namespace stm