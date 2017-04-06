/**
 * @file NmeaDevice.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_DEVICE_NMEA_DEVICE_H
#define TESEO_HAL_DEVICE_NMEA_DEVICE_H

#include "AbstractDevice.h"

namespace stm {
namespace device {

/**
 * @brief      NMEA Device manager
 */
class NmeaDevice :
	public AbstractDevice
{
public:
	NmeaDevice();
};

} // namespace device
} // namespace stm

#endif // TESEO_HAL_DEVICE_NMEA_DEVICE_H