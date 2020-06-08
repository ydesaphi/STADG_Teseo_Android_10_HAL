/*
 * This file is part of Teseo Android HAL
 *
 * Copyright (c) 2016-2020, STMicroelectronics - All Rights Reserved
 * Author(s): Baudouin Feildel <baudouin.feildel@st.com> for STMicroelectronics.
 *
 * License terms: Apache 2.0.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @brief HAL Life cycle manager
 * @file HalManager.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_HAL_MANAGER_H
#define TESEO_HAL_HAL_MANAGER_H

#include <stdlib.h>

#include <teseo/utils/optional.h>
#include <teseo/utils/Signal.h>

#include <teseo/utils/Gnss_1_0.h>

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

namespace geofencing {
class GeofencingManager;
} // namespace geofencing

namespace straw {
class StrawEngine;
} // namespace straw

namespace ril {
class Ril_If;
} // namespace ril

namespace ni {
class Ni_If;
} // namespace ril

namespace agps {
class Agps_If;
} // namespace agps


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

	geofencing::GeofencingManager * geofencingManager;

	straw::StrawEngine *rawMeasurement;
	stm::ril::Ril_If * rilIf;

	stm::ni::Ni_If * niIf;

	stm::agps::Agps_If * AgpsIf;


	void initUtils();

	void initDevice();

	void initStagps();

	void initGeofencing();

	void initRawMeasurement();
	void initAGpsIf();

	void initRilIf();

	void initNiIf();

	void initAssistance();

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
	int init(const sp<IGnssCallback>& cb);

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
