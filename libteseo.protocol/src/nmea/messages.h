/*
* This file is part of Teseo Android HAL
*
* Copyright (c) 2016-2017, STMicroelectronics - All Rights Reserved
* Author(s): Baudouin Feildel <baudouin.feildel@st.com> for STMicroelectronics.
*
* License terms: Apache 2.0.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
/**
 * @file messages.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_DECODER_NMEA_MESSAGES_H
#define TESEO_HAL_DECODER_NMEA_MESSAGES_H

#include <teseo/model/NmeaMessage.h>
#include <teseo/protocol/NmeaDecoder.h>
#include <teseo/device/AbstractDevice.h>

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

	/**
	 * @brief      --GSV decoder
	 *
	 * @param      dev   Device to update
	 * @param[in]  msg   GSV Message to decode
	 */
	static void gsv(AbstractDevice & dev, const NmeaMessage & msg);

	/**
	 * @brief      --GSA decoder
	 *
	 * @param      dev   Device to update
	 * @param[in]  msg   GSA Message to decode
	 */
	static void gsa(AbstractDevice & dev, const NmeaMessage & msg);

	/**
	 * @brief      PSTMSBAS decoder
	 *
	 * @param      dev   Device to update
	 * @param[in]  msg   PSTMSBAS Message to decode
	 */
	static void sbas(AbstractDevice & dev, const NmeaMessage & msg);

	/**
	 * @brief      PSTMVER decoder
	 *
	 * @param      dev   Device to update
	 * @param[in]  msg   PSTMVER Message to decode
	 */
	static void pstmver(AbstractDevice & dev, const NmeaMessage & msg);

	/**
	 * @brief      PSTMSTAGPS8PASSRTN decoder
	 *
	 * @param      dev   Device to update
	 * @param[in]  msg   PSTMSTAGPS8PASSRTN Message to decode
	 */
	static void pstmstagps8passrtn(AbstractDevice & dev, const NmeaMessage & msg);

	/**
	 * @brief      PSTMSTAGPSPASSRTN decoder
	 *
	 * @param      dev   Device to update
	 * @param[in]  msg   PSTMSTAGPSPASSRTN Message to decode
	 */
	 static void pstmstagpspassrtn(AbstractDevice & dev, const NmeaMessage & msg);

	/**
	 * @brief      PSTMSTAGPSSATSEEDOK and PSTMSTAGPSSATSEEDERROR decoder
	 *
	 * @param      dev   Device to update
	 * @param[in]  msg   PSTMSTAGPSSATSEEDOK/ PSTMSTAGPSSATSEEDERRORMessage to decode
	 */
	 static void pstmstagpssatseedresponse(AbstractDevice & dev, const NmeaMessage & msg);
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