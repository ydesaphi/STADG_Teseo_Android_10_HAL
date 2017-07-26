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
