/*
 * This file is part of Teseo Android HAL
 *
 * Copyright (c) 2016-2020, STMicroelectronics - All Rights Reserved
 * Author(s): Fabrice Deruy <fabrice.deruy@st.com> for STMicroelectronics.
 *
 * License terms: Apache 2.0.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TESEO_HAL_MODEL_GPS_STATE_H
#define TESEO_HAL_MODEL_GPS_STATE_H

#include <functional>

#include <teseo/utils/Gnss_2_0.h>

namespace stm {

class GpsState {

private:
    /**
     * 'mode' parameter should be one of GPS_POSITION_MODE_MS_BASED
     * or GPS_POSITION_MODE_STANDALONE.
     * It is allowed by the platform (and it is recommended) to fallback to
     * GPS_POSITION_MODE_MS_BASED if GPS_POSITION_MODE_MS_ASSISTED is passed in, and
     * GPS_POSITION_MODE_MS_BASED is supported.
     *
     *  #define GPS_POSITION_MODE_STANDALONE    0
     *  //AGPS MS-Based mode.
     *  #define GPS_POSITION_MODE_MS_BASED      1
     *  #define GPS_POSITION_MODE_MS_ASSISTED   2 //Not supported anymore
     */
    GnssPositionMode m_GpsMode;


    /** Requested recurrence mode for GPS operation. */
    GnssPositionRecurrence m_GpsPositionRecurrence;

   /**
     * min_interval represents the time between fixes in milliseconds.
     * preferred_accuracy represents the requested fix accuracy in meters.
     * preferred_time represents the requested time to first fix in milliseconds.
     */
    uint32_t m_min_interval;
    uint32_t m_preferred_accuracy;
    uint32_t m_preferred_time;

public:
    // Constructor
    GpsState();

    static GpsState * getInstance(void);

    GnssPositionMode GetGpsMode(void) const;

    void SetPositionMode(GnssPositionMode mode,
        GnssPositionRecurrence recurrence,
	    uint32_t minInterval,
	    uint32_t preferredAccuracy,
	    uint32_t preferredTime);

};

} // namespace stm

#endif // TESEO_HAL_MODEL_GPS_STATE_H

