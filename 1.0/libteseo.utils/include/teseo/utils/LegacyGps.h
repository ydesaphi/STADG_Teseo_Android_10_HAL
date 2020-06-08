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
 * @brief Legacy gps definition
 * @file LegacyGps.h
 * @author Muhamad Rahmady <muhamad.rahmady@st.com>
 * @copyright 2020, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_LEGACY_GPS_H
#define TESEO_HAL_LEGACY_GPS_H

/**
 * Name for the GPS XTRA interface.
 */
#define GPS_XTRA_INTERFACE                  "gps-xtra"
/**
 * Name for the GPS DEBUG interface.
 */
#define GPS_DEBUG_INTERFACE                 "gps-debug"
/**
 * Name for the AGPS interface.
 */
#define AGPS_INTERFACE                      "agps"
/**
 * Name of the Supl Certificate interface.
 */
#define SUPL_CERTIFICATE_INTERFACE          "supl-certificate"
/**
 * Name for NI interface
 */
#define GPS_NI_INTERFACE                    "gps-ni"
/**
 * Name for the AGPS-RIL interface.
 */
#define AGPS_RIL_INTERFACE                  "agps_ril"
/**
 * Name for the GPS_Geofencing interface.
 */
#define GPS_GEOFENCING_INTERFACE            "gps_geofencing"
/**
 * Name of the GPS Measurements interface.
 */
#define GPS_MEASUREMENT_INTERFACE           "gps_measurement"
/**
 * Name of the GPS navigation message interface.
 */
#define GPS_NAVIGATION_MESSAGE_INTERFACE    "gps_navigation_message"
/**
 * Name of the GNSS/GPS configuration interface.
 */
#define GNSS_CONFIGURATION_INTERFACE        "gnss_configuration"

/*
 * AGnssRil
 */
#define AGPS_RIL_REQUEST_REFLOC_CELLID  (1 << 0L)

#endif  // TESEO_HAL_LEGACY_GPS_H
