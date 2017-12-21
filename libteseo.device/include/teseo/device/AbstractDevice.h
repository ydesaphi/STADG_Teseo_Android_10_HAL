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
 * @file AbstractDevice.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_ABSTRACT_DEVICE_H
#define TESEO_HAL_ABSTRACT_DEVICE_H

#include <mutex>
#include <map>
#include <unordered_map>

#include <hardware/gps.h>

#include <teseo/utils/any.h>
#include <teseo/utils/result.h>
#include <teseo/utils/Signal.h>
#include <teseo/model/Message.h>
#include <teseo/model/NmeaMessage.h>
#include <teseo/model/Location.h>
#include <teseo/model/SatInfo.h>
#include <teseo/model/Version.h>
#include <teseo/model/Stagps.h>
#include <teseo/utils/Thread.h>
#include <teseo/model/ValueContainer.h>

namespace stm {
namespace decoder {
	class NmeaDecoder;
	namespace nmea {
		struct decoders;
	} // namespace nmea
} // namespace decoder

namespace device {

/**
 * @brief      Abstract Device manager
 * 
 * @details    An abstract device store the Teseo current state in the HAL. This state is updated by
 * the attached decoder. 
 */
class AbstractDevice :
	public Trackable
{
private:
	const static ByteVector nmeaSequenceStart;

	// ======================== Data Model =====================
	
	ValueContainer<GpsUtcTime> timestamp;

	ValueContainer<Location> location;

	ValueContainer<std::map<SatIdentifier, SatInfo>> satellites;

	ValueContainer<std::unordered_map<std::string, model::Version>> versions;

protected:

	// Allow NmeaDecoder to use emitNmea
	friend class stm::decoder::NmeaDecoder;

	// Allow decoding functions to update device data model
	friend struct stm::decoder::nmea::decoders;

	/**
	 * @brief      Abstract device constructor
	 * @details    The constructor is protected so it must be inherited to be instanciated.
	 */
	AbstractDevice();

	/**
	 * @brief      Sets the timestamp.
	 *
	 * @param[in]  timestamp  The timestamp
	 */
	void setTimestamp(GpsUtcTime timestamp);

	/**
	 * @brief      Set the device location.
	 *
	 * @param[in]  loc New device location
	 */
	void setLocation(const Location & loc);

	/**
	 * @brief      Add or update satellite
	 *
	 * @param[in]  sat New satellite information to store
	 */
	void addSatellite(const SatInfo & sat);

	/**
	 * @brief      Clear the satellite list
	 */
	void clearSatelliteList();

	void newVersionNumber(const model::Version & version);

	/**
	 * @brief      Emit a NMEA message
	 *
	 * @param[in]  nmea  The nmea message to emit
	 */
	void emitNmea(const NmeaMessage & nmea);

	/**
	 * @brief      Trigger device update
	 * @details    The device will emit signals to inform the HAL that its data has been updated.
	 */
	void update();

	/**
	 * @brief Trigger device update if sentence id is equal to trigger
	 */
	void updateIfStartSentenceId(const ByteVector & sentenceId);

public:

	/**
	 * @brief      Destructor
	 */
	virtual ~AbstractDevice() { }

	/**
	 * @brief      Gets the current utc-time as reported by the Teseo
	 *
	 * @return     The current utc-time or ValueStatus if not available.
	 */
	Result<GpsUtcTime, ValueStatus> getTimestamp() const;

	/**
	 * @brief      Gets the location.
	 *
	 * @return     Current location or ValueStatus if not available.
	 */
	Result<Location, ValueStatus> getLocation() const;

	/**
	 * @brief      Get one satellite from satellite list
	 *
	 * @param[in]  id Identifier of the satellite to get
	 *
	 * @return     SatInfo object related to the satellite identifier,
	 *             or ValueStatus if not available.
	 */
	Result<SatInfo, ValueStatus> getSatellite(const SatIdentifier & id) const;

	/**
	 * @brief      Gets version for product `productName`
	 *
	 * @param[in]  productName Name of the product to get the version from
	 *
	 * @return     The product version information or ValueStatus if not available.
	 */
	Result<model::Version, ValueStatus>
		getProductVersion(const std::string & productName) const;

	/**
	 * Request to send a message to the Teseo
	 *
	 * @param message The message to send
	 */
	void sendMessageRequest(const model::Message & message);

	/**
	 * @brief      Start the navigation
	 *
	 * @return     0 on success, 1 on failure
	 */
	int start();

	/**
	 * @brief      Stop the navigation
	 *
	 * @return     0 on success, 1 on failure
	 */
	int stop();

	void init();

	/**
	 * Signal sent when navigation starts
	 */
	Signal<int> startNavigation;

	/**
	 * Signal sent when navigation stops
	 */
	Signal<int> stopNavigation;

	/**
	 * NMEA signal
	 */
	Signal<void, GpsUtcTime, const NmeaMessage &> onNmea;

	/**
	 * Location update signal
	 */
	Signal<void, const Location &> locationUpdate;

	/**
	 * Satellite list update signal
	 */
	Signal<void, const std::map<SatIdentifier, SatInfo> &> satelliteListUpdate;

	Signal<void, GpsStatusValue> statusUpdate;

	Signal<void> requestUtcTime;

	Signal<void, const model::Version &> onVersionNumber;

	Signal<void, const AbstractDevice &, const model::Message &> sendMessage;

	Signal<void, model::Stagps8Answer, const std::vector<ByteVector> &> onStagps8Answer;

	Signal<void, model::StagpsAnswer, const std::vector<ByteVector> &> onStagpsAnswer;
};

} // namespace device
} // namespace stm

#endif // TESEO_HAL_ABSTRACT_DEVICE_H