#ifndef TESEO_HAL_PROTOCOL_NMEA_ENCODER_H
#define TESEO_HAL_PROTOCOL_NMEA_ENCODER_H

#include "IEncoder.h"

#include <teseo/model/Message.h>

namespace stm {
namespace protocol {

class NmeaEncoder : public IEncoder
{
public:
	NmeaEncoder();

	virtual ~NmeaEncoder();

	virtual void encode(model::MessageId id);
};

} // namespace protocol
} // namespace stm

#endif // TESEO_HAL_PROTOCOL_NMEA_ENCODER_H