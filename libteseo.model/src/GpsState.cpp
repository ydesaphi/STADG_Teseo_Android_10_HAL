/*
* This file is part of Teseo Android HAL
*
* Copyright (c) 2016-2018, STMicroelectronics - All Rights Reserved
* Author(s): Fabrice Deruy <fabrice.deruy@st.com> for STMicroelectronics.
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

#include <teseo/model/GpsState.h>

#define LOG_TAG "teseo_hal_GpsState"
#include <cutils/log.h>

static stm::GpsState GpsStateInternal;

static const char* gpsStateModeLookUpTable[] = 
{
    "GPS_POSITION_MODE_STANDALONE",
    "GPS_POSITION_MODE_MS_BASED"
};

namespace stm
{
    // Constructor
    GpsState::GpsState():m_GpsMode(GPS_POSITION_MODE_MS_BASED),
                         m_min_interval(1000)
    {

    }

    GpsState * GpsState::getInstance(void){
        return &GpsStateInternal;
    }

    GpsPositionMode GpsState::GetGpsMode(void) const{
        return this->m_GpsMode;
    }

    void GpsState::SetPositionMode(GpsPositionMode mode,
    GpsPositionRecurrence recurrence,
	uint32_t minInterval,
	uint32_t preferredAccuracy,
	uint32_t preferredTime)
    {
        //Unused parameters
        (void)(recurrence);
        (void)(preferredAccuracy);
        (void)(preferredTime);
        (void)(minInterval);

        this->m_GpsMode = mode;   

        ALOGV("Gps mode set to %s",gpsStateModeLookUpTable[static_cast<int>(mode)]);   
    }


} // namespace stm