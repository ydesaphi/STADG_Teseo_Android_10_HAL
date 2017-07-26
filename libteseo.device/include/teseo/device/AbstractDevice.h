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

#include <teseo/utils/result.h>
#include <teseo/utils/Signal.h>
#include <teseo/model/Message.h>
#include <teseo/model/NmeaMessage.h>
#include <teseo/model/Location.h>
#include <teseo/model/SatInfo.h>
#include <teseo/model/Version.h>
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
	 * @brief      Request version numbers from device
	 */
	void requestVersionNumbers();

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

	Signal<int> startNavigation;

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

	Signal<model::Version> onVersionNumber;

	Signal<void, model::MessageId> sendMessage;
};

} // namespace device
} // namespace stm

#endif // TESEO_HAL_ABSTRACT_DEVICE_H