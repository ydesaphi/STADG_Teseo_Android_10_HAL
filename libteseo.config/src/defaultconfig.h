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
 * @file defaultconfig.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_DEFAULT_CONFIG_H
#define TESEO_HAL_DEFAULT_CONFIG_H

#define CFG_DEF_DEVICE_TTY std::string("/dev/ttyAMA2")
#define CFG_DEF_DEVICE_SPEED 115200


#define CFG_DEF_DATA_ASSISTANCE_ENABLED false
#define CFG_DEF_STAGPS_ENABLE false

#define CFG_DEF_CONSTELLATIONS_GPS     true
#define CFG_DEF_CONSTELLATIONS_GLONASS true
#define CFG_DEF_CONSTELLATIONS_BEIDOU  true
#define CFG_DEF_CONSTELLATIONS_GALILEO true

#define CFG_DEF_STAGPS_PREDICTIVE_ENABLE    false
#define CFG_DEF_STAGPS_PREDICTIVE_HOST      std::string("")
#define CFG_DEF_STAGPS_PREDICTIVE_PORT      0
#define CFG_DEF_STAGPS_PREDICTIVE_VENDOR_ID std::string("")
#define CFG_DEF_STAGPS_PREDICTIVE_MODEL_ID  std::string("")
#define CFG_DEF_STAGPS_PREDICTIVE_DEVICE_ID std::string("")
#define CFG_DEF_STAGPS_PREDICTIVE_SEED_TYPE 14
#define CFG_DEF_STAGPS_PREDICTIVE_BASE_PATH std::string("")


#define CFG_DEF_STAGPS_REALTIME_ENABLE    false
#define CFG_DEF_STAGPS_REALTIME_HOST      std::string("")
#define CFG_DEF_STAGPS_REALTIME_PORT      0
#define CFG_DEF_STAGPS_REALTIME_VENDOR_ID std::string("")
#define CFG_DEF_STAGPS_REALTIME_MODEL_ID  std::string("")
#define CFG_DEF_STAGPS_REALTIME_DEVICE_ID std::string("")
#define CFG_DEF_STAGPS_REALTIME_BASE_PATH std::string("")

#endif // TESEO_HAL_DEFAULT_CONFIG_H