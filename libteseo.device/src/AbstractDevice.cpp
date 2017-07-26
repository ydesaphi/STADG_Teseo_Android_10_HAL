/**
 * @file AbstractDevice.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/device/AbstractDevice.h>

#define LOG_TAG "teseo_hal_AbstractDevice"
#include <cutils/log.h>
#include <time.h>

#include <teseo/utils/Wakelock.h>
#include <teseo/model/NmeaMessage.h>
#include <teseo/model/Message.h>

using namespace stm::model;

namespace stm {
namespace device {

const ByteVector AbstractDevice::nmeaSequenceStart {'G', 'G', 'A'};

AbstractDevice::AbstractDevice()
{ }

void AbstractDevice::init()
{
	statusUpdate(GPS_STATUS_NONE);
}

void AbstractDevice::setLocation(const Location & loc)
{
	location.set(loc);
}

void AbstractDevice::addSatellite(const SatInfo & sat)
{
	if(!satellites)
		satellites.set(std::map<SatIdentifier, SatInfo>());
	
	auto & sats = *satellites;
	sats[sat.getId()] = sat;
}

void AbstractDevice::clearSatelliteList()
{
	satellites->clear();
	satellites.invalidate();
}

Result<GpsUtcTime, ValueStatus> AbstractDevice::getTimestamp() const
{
	if(this->timestamp)
		return *timestamp;

	return timestamp.getStatus();
}

Result<Location, ValueStatus> AbstractDevice::getLocation() const
{
	if(this->location)
		return *location;

	return location.getStatus();
}

Result<SatInfo, ValueStatus> AbstractDevice::getSatellite(const SatIdentifier & id) const
{
	if(!satellites)
		return ValueStatus::NOT_AVAILABLE;

	auto & sats = *satellites;
	auto it = sats.find(id);

	if(it != sats.end())
	{
		return it->second;
	}
	else
	{
		return ValueStatus::NOT_AVAILABLE;
	}
}

void AbstractDevice::update()
{
	// Update location only if it is valid
	if(location->locationValidity())
		locationUpdate(location);

	// Trigger satellite list update
	satelliteListUpdate(this->satellites);
}

void AbstractDevice::updateIfStartSentenceId(const ByteVector & sentenceId)
{
	if(sentenceId == nmeaSequenceStart)
	{
		// Trigger updates
		update();

		// Clear data before starting new sequence
		this->clearSatelliteList();
	}
}

int AbstractDevice::start()
{
	ALOGI("Start navigation");

	// Acquire wakelock, request UTC time and update status
	utils::Wakelock::acquire();
	requestUtcTime();
	statusUpdate(GPS_STATUS_SESSION_BEGIN);

	// Start the navigation
	startNavigation();

	return 0;
}

int AbstractDevice::stop()
{
	ALOGI("Stop navigation");

	// Stop the navigation
	stopNavigation();

	// Release wakelock and update status
	utils::Wakelock::release();
	statusUpdate(GPS_STATUS_SESSION_END);

	return 0;
}

void AbstractDevice::setTimestamp(GpsUtcTime t)
{
	timestamp = t;
	location->timestamp(t);
}

void AbstractDevice::emitNmea(const NmeaMessage & nmea)
{
	onNmea(timestamp, nmea);
}

Result<Version, ValueStatus>
	AbstractDevice::getProductVersion(const std::string & productName) const
{
	if(versions->size() == 0)
		return ValueStatus::NOT_AVAILABLE;

	if(versions->count(productName) == 0)
		return ValueStatus::NOT_AVAILABLE;

	return versions->at(productName);
}

void AbstractDevice::requestVersionNumbers()
{
	sendMessage(model::MessageId::GetVersions);
}

} // namespace device
} // namespace stm