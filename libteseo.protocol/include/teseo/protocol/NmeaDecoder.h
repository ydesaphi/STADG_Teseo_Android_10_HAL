/**
 * @file NmeaDecoder.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_DECODER_NMEA_DECODER_H
#define TESEO_HAL_DECODER_NMEA_DECODER_H

#include <teseo/utils/ByteVector.h>
#include <teseo/device/AbstractDevice.h>

#include "AbstractDecoder.h"

namespace stm {
namespace decoder {
namespace nmea {

/**
 * @brief      Extract checksum from message
 *
 * @param[in]  bytes             The message ascii string
 * @param      multipleChecksum  Flag setted to true if there is multiple checksum in the message
 * @param      noChecksum        Flag setted to true if there is no checksum in the message
 * @param      invalidChar       Flag setted to true if there are invalid characters in the checksum
 *
 * @return     The extract checksum
 */
uint8_t extractChecksum(
	const ByteVector & bytes,
	bool & multipleChecksum,
	bool & noChecksum,
	bool & invalidChar);

/**
 * @brief      Calculates the checksum.
 *
 * @param[in]  bytes  The bytes
 *
 * @return     The checksum.
 */
uint8_t computeChecksum(const ByteVector & bytes);

/**
 * @brief      Validate NMEA checksum
 *
 * @param[in]  bytes             The message ascii string
 * @param      multipleChecksum  Flag set to true if there is multiple checksum in the message
 * @param      crc               Setted to the computed checksum
 *
 * @return     True if checksum is valid, false otherwise
 */
bool validateChecksum(const ByteVector & bytes, bool & multipleChecksum, uint8_t & crc);
}

/**
 * @brief      NMEA Decoder
 */
class NmeaDecoder :
	public AbstractDecoder
{
private:
	device::AbstractDevice & device;

protected:
	/**
	 * @brief      Decode one NMEA message
	 *
	 * @param[in]  bytes  The message as ascii string
	 */
	virtual void decode(ByteVectorPtr bytes);

public:
	/**
	 * @brief      Decoder constructor
	 *
	 * @param      device  The connected device
	 */
	NmeaDecoder(device::AbstractDevice & device);
};

} // namespace decoder
} // namespace stm

#endif // TESEO_HAL_DECODER_NMEA_DECODER_H