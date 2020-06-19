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
 * @file TalkerId.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/model/TalkerId.h>

namespace stm {
namespace model {

constexpr const char * talkerStrings[] = {
	"GP",
	"GA",
	"GL",
	"GN",
	"GB",
	"BD",
	"QZ",
	"PSTM",
	"INVALID"
};

const char * TalkerIdToString(TalkerId id)
{
	return talkerStrings[static_cast<uint8_t>(id)];
}

TalkerId ByteVectorToTalkerId(const ByteVector & bytes)
{
	if(bytes.size() < 2)
		return TalkerId::INVALID;

	uint8_t b = bytes[1];

	if(b == 'P') return TalkerId::GP;
	if(b == 'S') return TalkerId::PSTM;
	if(b == 'L') return TalkerId::GL;
	if(b == 'B') return TalkerId::GB;
	if(b == 'A') return TalkerId::GA;
	if(b == 'N') return TalkerId::GN;
	if(b == 'D') return TalkerId::BD;
	if(b == 'Z') return TalkerId::QZ;

	return TalkerId::INVALID;
}

} // namespace model
} // namespace stm