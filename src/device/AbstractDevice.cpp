/**
 * @file AbstractDevice.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include "AbstractDevice.h"

#define LOG_TAG "teseo_hal_AbstractDevice"
#include <cutils/log.h>
#include <time.h>

#include "../LocServiceProxy.h"

namespace stm {
namespace device {

const ByteVector AbstractDevice::nmeaSequenceStart {'G', 'G', 'A'};

AbstractDevice::AbstractDevice()
{
	stream = nullptr;
	decoder = nullptr;

	auto & gpsSignals = LocServiceProxy::gps::getSignals();
	gpsSignals.start.connect(SlotFactory::create(*this, &AbstractDevice::start));
	gpsSignals.stop.connect(SlotFactory::create(*this, &AbstractDevice::stop));


	onNmea.connect(SlotFactory::create(LocServiceProxy::gps::sendNmea));
	locationUpdate.connect(SlotFactory::create(LocServiceProxy::gps::sendLocationUpdate));
	satelliteListUpdate.connect(SlotFactory::create(LocServiceProxy::gps::sendSatelliteListUpdate));
	statusUpdate.connect(SlotFactory::create(LocServiceProxy::gps::sendStatusUpdate));

	statusUpdate(GPS_STATUS_NONE);
}

void AbstractDevice::connectStreamToDecoder()
{
	if(stream == nullptr)
	{
		ALOGE("Stream isn't set, won't connect.");
		return;
	}

	if(decoder == nullptr)
	{
		ALOGE("Decoder isn't set, won't connect.");
		return;
	}

	stream->onNewBytes.connect(SlotFactory::create(*decoder, &decoder::AbstractDecoder::onNewBytes));
}

void AbstractDevice::setStream(stream::IStream * s)
{
	if(s == nullptr)
	{
		ALOGW("Setting stream to nullptr");
	}

	stream = s;
}

void AbstractDevice::setDecoder(decoder::AbstractDecoder * d)
{
	if(d == nullptr)
	{
		ALOGW("Setting decoder to nullptr");
	}

	decoder = d;
}

void AbstractDevice::update()
{
	if(location.locationValidity())
		locationUpdate(location);

	satelliteListUpdate(this->satellites);
}

void AbstractDevice::updateIfStartSentenceId(const ByteVector & sentenceId)
{
	if(sentenceId == nmeaSequenceStart)
	{
		update();

		// Clear data before starting new sequence
		this->satellites.clear();
	}
}

/*
void AbstractDevice::updateSatelliteList()
{
	int gpsSats = 0;
	int gloSats = 0;
	int galSats = 0;
	int beiSats = 0;
	int otherSats = 0;
	int totalSats = 0;

	auto action = [&] (auto & p) {
		switch(p.second.getId().getConstellation()) {
			case Constellation::Gps:     gpsSats++; break;
			case Constellation::Glonass: gloSats++; break;
			case Constellation::Galileo: galSats++; break;
			case Constellation::Beidou:  beiSats++; break;
			default: otherSats++; break;
		}
		totalSats++;
	};

	std::for_each(this->satellites.begin(), this->satellites.end(), action);

	ALOGI("Send satellite list: %d satellites, %d gps, %d glonass, %d galileo, %d beidou, %d others",
		totalSats, gpsSats, gloSats, galSats, beiSats, otherSats);
	satelliteListUpdate(this->satellites);
}
*/

int AbstractDevice::start()
{
	ALOGI("Start navigation");
	LocServiceProxy::gps::acquireWakelock();
	LocServiceProxy::gps::requestUtcTime();
	decoder->start();
	stream->startReading();
	statusUpdate(GPS_STATUS_SESSION_BEGIN);

	return 0;
}

int AbstractDevice::stop()
{
	ALOGI("Stop navigation");

	stream->stopReading();
	decoder->stop();
	LocServiceProxy::gps::releaseWakelock();
	statusUpdate(GPS_STATUS_SESSION_END);

	return 0;
}

void AbstractDevice::setTimestamp(GpsUtcTime t)
{
	timestamp = t;
	location.timestamp(t);
}

void AbstractDevice::emitNmea(const NmeaMessage & nmea)
{
	onNmea(timestamp, nmea);
}

} // namespace device
} // namespace stm