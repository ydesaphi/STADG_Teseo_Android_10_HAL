/**
 * @file gps.c
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include "LocServiceProxy.h"

static struct hw_module_methods_t gps_module_methods = {
	.open = &stm::LocServiceProxy::openDevice
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
