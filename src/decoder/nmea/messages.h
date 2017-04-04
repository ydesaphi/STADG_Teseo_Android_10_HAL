#ifndef TESEO_HAL_DECODER_NMEA_MESSAGES_H
#define TESEO_HAL_DECODER_NMEA_MESSAGES_H

#include "../../model/NmeaMessage.h"
#include "../NmeaDecoder.h"
#include "../../device/AbstractDevice.h"

namespace stm {
namespace decoder {
namespace nmea {

using namespace stm::device;

struct decoders {
private:
	decoders() { }

public:
	static void gga(AbstractDevice & dev, const NmeaMessage & msg);
	static void vtg(AbstractDevice & dev, const NmeaMessage & msg);
};

typedef void (*MessageDecoder)(AbstractDevice & dev, const NmeaMessage &);

void decode(AbstractDevice & dev, const NmeaMessage & msg);

MessageDecoder getMessageDecoder(const NmeaMessage & msg);

} // namespace nmea
} // namespace decoder
} // namespace stm

#endif // TESEO_HAL_DECODER_NMEA_MESSAGES_H