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
#include <teseo/model/Constellations.h>

namespace stm {
namespace ConstellationUtils {

using namespace stm::model;

GnssConstellationType toAndroidType(Constellation value)
{
	return static_cast<GnssConstellationType>(value);
}

Constellation convert(uint8_t value)
{
	if(value > static_cast<uint8_t>(Constellation::Galileo))
		return Constellation::Unknown;

	return static_cast<Constellation>(value);
}

Constellation convert(TalkerId id)
{
	switch(id)
	{
		case TalkerId::GP: return Constellation::Gps;
		case TalkerId::GA: return Constellation::Galileo;
		case TalkerId::GL: return Constellation::Glonass;
		case TalkerId::GN: return Constellation::Unknown;
		case TalkerId::GB: return Constellation::Beidou;
		case TalkerId::BD: return Constellation::Beidou;
		case TalkerId::QZ: return Constellation::Qzss;
		default:           return Constellation::Unknown;
	}
}

} // namespace ConstellationUtils
} // namespace stm