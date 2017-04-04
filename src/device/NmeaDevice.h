#ifndef TESEO_HAL_DEVICE_NMEA_DEVICE_H
#define TESEO_HAL_DEVICE_NMEA_DEVICE_H

#include "AbstractDevice.h"

namespace stm {
namespace device {

class NmeaDevice :
	public AbstractDevice
{
public:
	NmeaDevice();
};

} // namespace device
} // namespace stm

#endif // TESEO_HAL_DEVICE_NMEA_DEVICE_H