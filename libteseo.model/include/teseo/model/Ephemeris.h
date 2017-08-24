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
#ifndef TESEO_HAL_MODEL_EPHEMERIS_H
#define TESEO_HAL_MODEL_EPHEMERIS_H

#include <cstdint>
#include <teseo/utils/ByteVector.h>

namespace stm::model {

struct GpsEphemerisData
{   // Fields are grouped in 32 bits groups
	uint32_t week                 : 16;
	uint32_t toe                  : 16;
	// --
	uint32_t toc                  : 16;
	uint32_t iode1                : 8;
	uint32_t iode2                : 8;
	// --
	uint32_t iodc                 : 10;
	uint32_t i_dot                : 14;
	uint32_t spare1               : 8;
	// --
	uint32_t omega_dot            : 24;
	uint32_t ephems_n             : 2;
	uint32_t time_distance_h      : 6;
	// --
	uint32_t crs                  : 16;
	uint32_t crc                  : 16;
	// --
	uint32_t cus                  : 16;
	uint32_t cuc                  : 16;
	// --
	uint32_t cis                  : 16;
	uint32_t cic                  : 16;
	// --
	uint32_t motion_difference    : 16;
	uint32_t age_h                : 10;
	uint32_t spare3               : 6;
	// --
	uint32_t inclination          : 32;
	// --
	uint32_t eccentricity         : 32;
	// --
	uint32_t root_a               : 32;
	// --
	uint32_t mean_anomaly         : 32;
	// --
	uint32_t omega_zero           : 32;
	// --
	uint32_t perigee              : 32;
	// --
	uint32_t time_group_delay     : 8;
	uint32_t af2                  : 8;
	uint32_t af1                  : 16;
	// --
	uint32_t af0                  : 22;
	uint32_t subframe1_available  : 1;
	uint32_t subframe2_available  : 1;
	uint32_t subframe3_available  : 1;
	uint32_t available            : 1;
	uint32_t health               : 1;
	uint32_t predicted            : 1;
	uint32_t accuracy             : 4;
	// --
};


union GpsEphemeris {
	GpsEphemerisData d;
	uint8_t          raw[sizeof(GpsEphemerisData)];	

	GpsEphemeris(): raw({0}) { }
};

} // namespace stm::model

/*
 * Ephemeris serializer
 */
stm::ByteVector operator << (stm::ByteVector && bv, const stm::model::GpsEphemeris & eph);
stm::ByteVector & operator << (stm::ByteVector & bv, const stm::model::GpsEphemeris & eph);
 

#endif // TESEO_HAL_MODEL_EPHEMERIS_H