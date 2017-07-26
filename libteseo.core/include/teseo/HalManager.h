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

#include <teseo/utils/optional.h>
#include <teseo/utils/Signal.h>

namespace stm {

namespace device {
class AbstractDevice;
} // namespace device

namespace decoder {
class AbstractDecoder;
} // namespace decoder

namespace protocol {
class IEncoder;
} // namespace IEncoder

namespace stream {
class IStream;
class IByteStream;
} // namespace stream

namespace stagps {
class StagpsEngine;
} // namespace stagps

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

	decoder::AbstractDecoder * decoder;

	protocol::IEncoder * encoder;

	stream::IStream * stream;

	stream::IByteStream * byteStream;

	stagps::StagpsEngine * stagpsEngine;

	void initUtils();
	
	void initDevice();

	void initStagps();

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