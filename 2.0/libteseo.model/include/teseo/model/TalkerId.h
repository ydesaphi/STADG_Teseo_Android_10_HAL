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
 * @file TalkerId.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_MODEL_TALKER_ID_H
#define TESEO_HAL_MODEL_TALKER_ID_H

#include <teseo/utils/ByteVector.h>

namespace stm {
namespace model {

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

constexpr std::size_t TalkerIdByteCount(TalkerId id)
{
	switch(id)
	{
		case TalkerId::GP:
		case TalkerId::GA:
		case TalkerId::GL:
		case TalkerId::GN:
		case TalkerId::GB:
		case TalkerId::BD:
		case TalkerId::QZ:
			return 2;

		case TalkerId::PSTM:
			return 4;

		default:
			return 7;
	}
}

constexpr ByteArray<2> StdTalkerIdToByteArray(TalkerId id)
{
	switch(id)
	{
		case TalkerId::GP: return BA("GP");
		case TalkerId::GA: return BA("GA");
		case TalkerId::GL: return BA("GL");
		case TalkerId::GN: return BA("GN");
		case TalkerId::GB: return BA("GB");
		case TalkerId::BD: return BA("BD");
		case TalkerId::QZ: return BA("QZ");

		default:
			return BA("ER");
	}
}

constexpr ByteArray<4> ProprietaryTalkerIdToByteArray(TalkerId id)
{
	switch(id)
	{
		case TalkerId::PSTM: return BA("PSTM");
		
		default:
			return BA("ERRO");
	}
}

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

} // namespace model
} // namespace stm

#endif // TESEO_HAL_MODEL_TALKER_ID_H