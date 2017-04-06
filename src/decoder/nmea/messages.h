/**
 * @file messages.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_DECODER_NMEA_MESSAGES_H
#define TESEO_HAL_DECODER_NMEA_MESSAGES_H

#include "../../model/NmeaMessage.h"
#include "../NmeaDecoder.h"
#include "../../device/AbstractDevice.h"

namespace stm {
namespace decoder {
namespace nmea {

using namespace stm::device;

/**
 * @brief      NMEA Message decoders
 * 
 * @details    This is declared as a struct to allow friend declaration with AbstractDevice
 */
struct decoders {
private:
	decoders() { }

public:
	/**
	 * @brief      --GGA decoder
	 *
	 * @param      dev   Device to update
	 * @param[in]  msg   GGA Message to decode
	 */
	static void gga(AbstractDevice & dev, const NmeaMessage & msg);

	/**
	 * @brief      --VTG decoder
	 *
	 * @param      dev   Device to update
	 * @param[in]  msg   VTG Message to decode
	 */
	static void vtg(AbstractDevice & dev, const NmeaMessage & msg);
};

/**
 * @brief      Decode an NMEA message
 *
 * @param      dev   The device to update while decoding
 * @param[in]  msg   The message to decode
 */
void decode(AbstractDevice & dev, const NmeaMessage & msg);

} // namespace nmea
} // namespace decoder
} // namespace stm

#endif // TESEO_HAL_DECODER_NMEA_MESSAGES_H