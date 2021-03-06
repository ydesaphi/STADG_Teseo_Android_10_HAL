/*
 * This file is part of Teseo Android HAL
 *
 * Copyright (c) 2016-2020, STMicroelectronics - All Rights Reserved
 * Author(s): Baudouin Feildel <baudouin.feildel@st.com> for STMicroelectronics.
 *
 * License terms: Apache 2.0.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <teseo/model/Constellations.h>

namespace stm {
namespace ConstellationUtils {

using namespace stm::model;

GnssConstellationType convert(uint8_t value)
{
	if(value > static_cast<uint8_t>(GnssConstellationType::GALILEO))
		return GnssConstellationType::UNKNOWN;

	return static_cast<GnssConstellationType>(value);
}

GnssConstellationType convert(TalkerId id)
{
	switch(id)
	{
		case TalkerId::GP: return GnssConstellationType::GPS;
		case TalkerId::GA: return GnssConstellationType::GALILEO;
		case TalkerId::GL: return GnssConstellationType::GLONASS;
		case TalkerId::GN: return GnssConstellationType::UNKNOWN;
		case TalkerId::GB: return GnssConstellationType::BEIDOU;
		case TalkerId::BD: return GnssConstellationType::BEIDOU;
		case TalkerId::QZ: return GnssConstellationType::QZSS;
		default:           return GnssConstellationType::UNKNOWN;
	}
}

} // namespace ConstellationUtils
} // namespace stm
