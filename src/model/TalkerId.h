/**
 * @file TalkerId.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_MODEL_TALKER_ID_H
#define TESEO_HAL_MODEL_TALKER_ID_H

#include "../utils/ByteVector.h"

namespace stm {

/**
 * @brief      TalkerId enumeration
 */
enum class TalkerId : uint8_t {
	GP   = 0, ///< Global Positioning System (GPS)
	GA   = 1, ///< Galileo Positioning System
	GL   = 2, ///< GLONASS Receiver
	GN   = 3, ///< Global Navigation Satellite System (GNSS)
	GB   = 4, ///< Beidou
	BD   = 5, ///< Beidou
	QZ   = 6, ///< QZSS
	PSTM = 7, ///< Proprietary STM
	INVALID
};

/**
 * @brief      Convert talker id to string
 *
 * @param[in]  id    The identifier
 *
 * @return     The talker id as a string
 */
const char * TalkerIdToString(TalkerId id);

/**
 * @brief      Convert ascii byte vector to talker id value
 *
 * @param[in]  bytes  The bytes
 *
 * @return     The talker id value
 */
TalkerId ByteVectorToTalkerId(const ByteVector & bytes);

} // namespace stm

#endif // TESEO_HAL_MODEL_TALKER_ID_H