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
 * @file converters.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/model/FixQuality.h>
#include <teseo/model/FixAndOperatingModes.h>

namespace stm {

FixQuality FixQualityFromInt(uint8_t qual)
{
	if(qual < 0 || 8 < qual)
		return FixQuality::Invalid;

	return static_cast<FixQuality>(qual);
}

namespace model {

FixMode FixModeFromChar(char ch)
{
	return ch == '2' ? FixMode::_2D :
	       ch == '3' ? FixMode::_3D :
	                   FixMode::NoFix;
}

FixMode FixModeFromInt(uint8_t value)
{
	return value == 2 ? FixMode::_2D :
	       value == 3 ? FixMode::_3D :
	                    FixMode::NoFix;
}
}
}