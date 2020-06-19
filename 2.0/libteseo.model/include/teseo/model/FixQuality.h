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
 * @file FixQuality.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_MODEL_FIX_QUALITY
#define TESEO_HAL_MODEL_FIX_QUALITY

#include <cstdint>

namespace stm {

/**
 * @brief      Fix quality enumeration
 */
enum class FixQuality : uint8_t
{
	Invalid           = 0, ///< Invalid Fix or not available
	GPS               = 1, ///< GPS Fix
	DGPS              = 2, ///< Differential GPS Fix
	PPS               = 3, ///< PPS Fix
	RealTimeKinematic = 4, ///< Real Time Kinematic
	FloatRTK          = 5, ///< Float RTK
	DeadReckoning     = 6, ///< Estimated (Dead-Reckoning)
	ManualInputMode   = 7, ///< Manual Input Mode
	SimulationMode    = 8, ///< Simulation Mode
};

/**
 * @brief      Convert integer to fix quality
 *
 * @param[in]  qual  The fix quality integer value
 *
 * @return     The fix quality enumeration value
 */
FixQuality FixQualityFromInt(uint8_t qual);

} // namespace stm

#endif // TESEO_HAL_MODEL_FIX_QUALITY