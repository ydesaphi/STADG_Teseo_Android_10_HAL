/*
* This file is part of Teseo Android HAL
*
* Copyright (c) 2016-2017, STMicroelectronics - All Rights Reserved
* Author(s): Yoann Desaphi <yoann.desaphi@st.com> for STMicroelectronics.
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
#ifndef TESEO_HAL_MODEL_DR_INFO_H
#define TESEO_HAL_MODEL_DR_INFO_H

#include <teseo/model/FixQuality.h>
#include <teseo/model/FixAndOperatingModes.h>

namespace stm {

using namespace model;

enum class ValueDrStatus : uint8_t {
	NO_FIX			= 0,
	DR_ONLY_FIX		= 1,
	GNSS_2D_FIX		= 2,
	GNSS_3D_FIX		= 3,
	GNSS_AND_DR_FIX		= 4,
	TIME_ONLY_FIX		= 5
};

class DrInfo {
private:
	/**
	 * DR status: GNSS fix quality including DR status
	 */
	ValueDrStatus drStatus;

	FixQuality fixQual;

	FixMode fixMode;

	bool isCalib;

	void updateDrStatus();

public:
	DrInfo();

	ValueDrStatus getStatus() const;

	void setGgaFixQual(FixQuality qual);

	void setGsaFixMode(FixMode mode);

	void setDrcalIsCalib(bool calib);
};

} // namespace stm

#endif // TESEO_HAL_MODEL_DR_INFO_H
