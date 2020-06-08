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
#ifndef TESEO_HAL_MODEL_CONSTELLATIONS
#define TESEO_HAL_MODEL_CONSTELLATIONS

#include <hardware/gps.h>

#include "TalkerId.h"

namespace stm {

enum class Constellation : GnssConstellationType {
	Unknown = GNSS_CONSTELLATION_UNKNOWN,
	Gps     = GNSS_CONSTELLATION_GPS,
	Sbas    = GNSS_CONSTELLATION_SBAS,
	Glonass = GNSS_CONSTELLATION_GLONASS,
	Qzss    = GNSS_CONSTELLATION_QZSS,
	Beidou  = GNSS_CONSTELLATION_BEIDOU,
	Galileo = GNSS_CONSTELLATION_GALILEO
};

namespace ConstellationUtils {

GnssConstellationType toAndroidType(Constellation value);

Constellation convert(uint8_t value);

Constellation convert(model::TalkerId id);

}

} // namespace stm

#endif // TESEO_HAL_MODEL_CONSTELLATIONS