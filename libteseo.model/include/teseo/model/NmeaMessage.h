/**
 * @file NmeaMessage.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_NMEA_MESSAGE_H
#define TESEO_HAL_NMEA_MESSAGE_H

#include <vector>
#include <string>
#include <teseo/utils/ByteVector.h>

#include "TalkerId.h"

namespace stm {

/**
 * @brief      NMEA message wrapper
 */
struct NmeaMessage {
public:
	NmeaMessage(
		const model::TalkerId talkerId, const ByteVector & sentenceId,
		const std::vector<ByteVector> & parameters, const uint8_t crc);

	NmeaMessage(const NmeaMessage & other);

	NmeaMessage(NmeaMessage && other);

	const model::TalkerId talkerId;

	const ByteVector sentenceId;

	const std::vector<const ByteVector> parameters;

	const uint8_t crc;

	/**
	 * @brief      Returns a string representation of the object.
	 *
	 * @return     String representation of the object.
	 */
	const std::string & toString() const;

	/**
	 * @brief      Returns a C-string representation of the object.
	 *
	 * @return     C-String representation of the object.
	 */
	const char * toCString() const;

private:
	std::string asString;

	void updateString();
};

} // namespace stm

#endif // TESEO_HAL_NMEA_MESSAGE_H