#ifndef TESEO_HAL_DECODER_NMEA_DECODER_H
#define TESEO_HAL_DECODER_NMEA_DECODER_H

#include "IDecoder.h"
#include "../utils/ByteVector.h"
#include "../device/AbstractDevice.h"

namespace stm {
namespace decoder {
namespace nmea {

uint8_t extractChecksum(
	const ByteVector & bytes,
	bool & multipleChecksum,
	bool & noChecksum,
	bool & invalidChar);

uint8_t computeChecksum(const ByteVector & bytes);

bool validateChecksum(const ByteVector & bytes, bool & multipleChecksum, uint8_t & crc);

}

class NmeaDecoder :
	public IDecoder
{
private:
	device::AbstractDevice & device;

protected:
	virtual void decode(ByteVectorPtr bytes);

public:
	NmeaDecoder(device::AbstractDevice & device);
};

} // namespace decoder
} // namespace stm

#endif // TESEO_HAL_DECODER_NMEA_DECODER_H