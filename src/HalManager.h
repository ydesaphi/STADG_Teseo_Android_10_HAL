/**
 * @brief HAL Life cycle manager
 * @file HalManager.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_HAL_MANAGER_H
#define TESEO_HAL_HAL_MANAGER_H

#include <hardware/hardware.h>
#include <hardware/gps.h>
#include <stdlib.h>

#include "Signal.h"
#include "device/AbstractDevice.h"

namespace stm {

/**
 * @brief      HAL Manager class
 * 
 * @details    This class is responsible of:
 * - The HAL initialization,
 * - The virtual device creation,
 * - The HAL cleanup
 */
class HalManager :
	public Trackable
{
private:
	Signal<void, uint32_t> setCapabilites;

	device::AbstractDevice * device;

public:
	HalManager();

	~HalManager();

	/**
	 * @brief      HAL Initializer
	 * 
	 * @details    The initializer create a virtual device using the gps.conf configuration file
	 * and send the HAL capabilities to the android platform.
	 *
	 * @param      cb    Unused
	 *
	 * @return     0 on success, 1 on failure
	 */
	int init(GpsCallbacks * cb);

	/**
	 * @brief      HAL Cleanup
	 */
	void cleanup();

	static HalManager & getInstance();

private:
	static HalManager instance;
};

}

#endif // TESEO_HAL_HAL_MANAGER_H