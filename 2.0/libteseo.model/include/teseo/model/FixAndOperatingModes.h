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
 * @file FixAndOperatingModes.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_MODEL_FIX_AND_OPERATING_MODES_H
#define TESEO_HAL_MODEL_FIX_AND_OPERATING_MODES_H

#include <cstdint>

namespace  stm::model {

enum class OperatingMode : uint8_t {
	Auto   = 'A',
	Manual = 'M'
};

enum class FixMode : uint8_t {
	NoFix  = 1,
	_2D    = 2,
	_3D    = 3
};

FixMode FixModeFromChar(char ch);

FixMode FixModeFromInt(uint8_t value);

} // namespace stm::model

#endif // TESEO_HAL_MODEL_FIX_AND_OPERATING_MODES_H