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
#ifndef TESEO_HAL_MODEL_MESSAGE
#define TESEO_HAL_MODEL_MESSAGE

#include <tuple>
#include <utility>
#include <type_traits>

#include <teseo/utils/ByteVector.h>
#include <teseo/model/TalkerId.h>

namespace stm {
namespace model {

enum class MessageId {
	/**
	 * Get version numbers from Teseo device
	 */
	GetVersions,

	/**
	 * Request ST-AGPS predictive password
	 * Parameters:
	 * - Vendor id
	 * - Device id
	 */
	Stagps_PasswordGenerate,

	/**
	 * Request ST-AGPS real-time password
	 * Parameters:
	 * - Vendor id
	 * - Model id
	 * - Device id
	 */
	Stagps8_PasswordGenerate,

	/**
	 * Send ephemeris for Real-Time Assisted GNSS
	 * Parameters:
	 * - Satellite Id
	 * - Size of data
	 * - Data
	 */
	Stagps_RealTime_Ephemeris,

	/**
	 * Send almanac for Real-Time Assisted GNSS
	 * Parameters:
	 * - Satellite Id
	 * - Size of data
	 * - Data
	 */
	 Stagps_RealTime_Almanac,

	/**
	 * Send seed for PGPS7 Assisted GNSS
	 * Parameters:
	 * - Satellite Id
	 * - Size of data
	 * - Data
	 */
	Stagps_PGPS7_Seed,

};

struct Message {
	MessageId id;
	std::vector<ByteVector> parameters;
};

} // namespace model
} // namespace stm

#endif // TESEO_HAL_MODEL_MESSAGE