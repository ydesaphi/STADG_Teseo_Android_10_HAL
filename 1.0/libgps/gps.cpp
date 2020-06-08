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
 * @brief Entry point of the HAL
 * @file gps.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <hardware/hardware.h>
#include <hardware/gps.h>
#include <teseo/teseo.h>

static struct hw_module_methods_t gps_module_methods = {
	.open = &teseo_hal_open_device
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
	.tag           = HARDWARE_MODULE_TAG,
	.version_major = 1,
	.version_minor = 0,
	.id            = GPS_HARDWARE_MODULE_ID,
	.name          = "Teseo GNSS Module",
	.author        = "STMicroelectronics",
	.methods       = &gps_module_methods,
};
