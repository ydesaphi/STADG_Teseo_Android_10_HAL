#ifndef TESEO_HAL_HAL_MANAGER_H
#define TESEO_HAL_HAL_MANAGER_H

#include <hardware/hardware.h>
#include <hardware/gps.h>
#include <stdlib.h>

#include "Signal.h"
#include "device/AbstractDevice.h"

namespace stm {

class HalManager :
	public Trackable
{
private:
	Signal<void, uint32_t> setCapabilites;

	device::AbstractDevice * device;

public:
	HalManager();

	~HalManager();

	int init(GpsCallbacks * cb);

	void cleanup();

	static HalManager & getInstance();

private:
	static HalManager instance;
};

}

#endif // TESEO_HAL_HAL_MANAGER_H