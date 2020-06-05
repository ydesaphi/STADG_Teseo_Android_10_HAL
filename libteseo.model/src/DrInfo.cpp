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
#include <teseo/model/DrInfo.h>

#define LOG_TAG "teseo_hal_DrInfo"
#include <log/log.h>

namespace stm {

DrInfo::DrInfo() :
	drStatus(ValueDrStatus::NO_FIX),
	fixQual(FixQuality::Invalid),
	fixMode(FixMode::NoFix),
	isCalib(false)
{ }

ValueDrStatus DrInfo::getStatus() const
{
	return this->drStatus;
}

void DrInfo::updateDrStatus()
{
	switch(this->fixQual)
	{
		case FixQuality::Invalid:
			this->drStatus = ValueDrStatus::NO_FIX;
			break;
		case FixQuality::DeadReckoning:
			this->drStatus = ValueDrStatus::DR_ONLY_FIX;
			break;
		case FixQuality::GPS:
		case FixQuality::DGPS:
			if((this->fixMode == FixMode::_3D || this->fixMode == FixMode::_2D) && this->isCalib)
			{
				this->drStatus = ValueDrStatus::GNSS_AND_DR_FIX;
			}
			else if(this->fixMode == FixMode::_3D)
			{
				this->drStatus = ValueDrStatus::GNSS_3D_FIX;
			}
			else if(this->fixMode == FixMode::_2D)
			{
				this->drStatus = ValueDrStatus::GNSS_2D_FIX;
			}
			else
			{
				ALOGW("DrInfo: Invalid case");
			}
			break;
		case FixQuality::PPS:
			this->drStatus = ValueDrStatus::TIME_ONLY_FIX;
			break;
		default:
			ALOGW("DrInfo: Unexpected fix quality value");
	}
}

void DrInfo::setGgaFixQual(FixQuality qual)
{
	if(qual < FixQuality::Invalid || qual > FixQuality::SimulationMode)
	{
		ALOGE("DrInfo: invalid fix quality value %hhu.", qual);
	}
	else
	{
		this->fixQual = qual;
	}

	updateDrStatus();
}

void DrInfo::setGsaFixMode(FixMode mode)
{
	if(mode < FixMode::NoFix || mode > FixMode::_3D)
	{
		ALOGE("DrInfo: invalid fix mode value %hhu.", mode);
	}
	else
	{
		this->fixMode = mode;
	}

	updateDrStatus();
}

void DrInfo::setDrcalIsCalib(bool calib)
{
	this->isCalib = calib;

	updateDrStatus();
}

} // namespace stm
