#ifndef TESEO_HAL_NMEA_MESSAGE_H
#define TESEO_HAL_NMEA_MESSAGE_H

#include <vector>
#include <string>
#include "../utils/ByteVector.h"
#include "TalkerId.h"

namespace stm {

struct NmeaMessage {
public:
	NmeaMessage(
		const TalkerId talkerId, const ByteVector & sentenceId,
		const std::vector<ByteVector> & parameters, const uint8_t crc);

	NmeaMessage(const NmeaMessage & other);

	NmeaMessage(NmeaMessage && other);

	const TalkerId talkerId;

	const ByteVector sentenceId;

	const std::vector<const ByteVector> parameters;

	const uint8_t crc;

	const std::string & toString() const;

	const char * toCString() const;

private:
	std::string asString;

	void updateString();
};

} // namespace stm

#endif // TESEO_HAL_NMEA_MESSAGE_H