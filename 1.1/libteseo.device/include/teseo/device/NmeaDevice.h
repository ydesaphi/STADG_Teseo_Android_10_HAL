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
 * @file NmeaDevice.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_DEVICE_NMEA_DEVICE_H
#define TESEO_HAL_DEVICE_NMEA_DEVICE_H

#include "AbstractDevice.h"

namespace stm {
namespace stream {
class IStream;
}

namespace device {

/**
 * @brief      NMEA Device manager
 */
class NmeaDevice :
	public AbstractDevice
{ };

} // namespace device
} // namespace stm

#endif // TESEO_HAL_DEVICE_NMEA_DEVICE_H