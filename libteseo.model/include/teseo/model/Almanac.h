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
#ifndef TESEO_HAL_MODEL_ALMANAC_H
#define TESEO_HAL_MODEL_ALMANAC_H

#include <teseo/utils/ByteVector.h>

namespace stm::model {

struct GpsAlmanacData
{   // Fields are grouped in 32 bits groups
	uint32_t satid           : 8;
	uint32_t week            : 16;
	uint32_t toa             : 8;
	// --
	uint32_t eccentricity    : 16;
	uint32_t delta_i         : 16;
	// --
	uint32_t omega_dot       : 16;
	uint32_t spare0          : 16;
	// --
	uint32_t root_a          : 24;
	uint32_t spare1          : 8;
	// --
	uint32_t omega_zero      : 24;
	uint32_t spare2          : 8;
	// --
	uint32_t perigee         : 24;
	uint32_t spare3          : 8;
	// --
	uint32_t mean_anomaly    : 24;
	uint32_t spare4          : 8;
	// --
	uint32_t af0             : 11;
	uint32_t af1             : 11;
	uint32_t health          : 1;
	uint32_t available       : 1;
	uint32_t spare5          : 8;
	// --
	uint32_t spare6          : 32;
	// --
	uint32_t spare7          : 32;
	// --
};

union GpsAlmanac {
	GpsAlmanacData d;
	uint8_t        raw[sizeof(GpsAlmanacData)];

	GpsAlmanac(): raw({0}) { }
};

} // namespace stm::model

/*
 * Ephemeris serializer
 */

stm::ByteVector operator << (stm::ByteVector && bv, const stm::model::GpsAlmanac & alm);

stm::ByteVector & operator << (stm::ByteVector & bv, const stm::model::GpsAlmanac & alm);

#endif // TESEO_HAL_MODEL_ALMANAC_H