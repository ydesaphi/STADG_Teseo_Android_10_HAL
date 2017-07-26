/**
 * @brief HAL Life cycle manager
 * @file HalManager.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/HalManager.h>

#define LOG_TAG "teseo_hal_HalManager"
#include <cutils/log.h>

#include <teseo/config/config.h>
#include <teseo/utils/Time.h>
#include <teseo/utils/Wakelock.h>

#include <teseo/utils/IByteStream.h>
#include <teseo/utils/IStream.h>
#include <teseo/device/AbstractDevice.h>
#include <teseo/protocol/AbstractDecoder.h>

#include <teseo/utils/UartByteStream.h>
#include <teseo/utils/NmeaStream.h>
#include <teseo/protocol/NmeaDecoder.h>
#include <teseo/device/NmeaDevice.h>
#include <teseo/protocol/NmeaEncoder.h>

#include <teseo/LocServiceProxy.h>

#ifdef STAGPS_ENABLED
#include <teseo/stagps/stagps.h>
#endif

namespace stm {

using namespace stm::device;

HalManager HalManager::instance;

HalManager::HalManager() :
	setCapabilites("HalManager::setCapabilites")
{
	ALOGI("Create HAL manager");

	device = nullptr;

	setCapabilites.connect(SlotFactory::create(&(LocServiceProxy::gps::sendCapabilities)));
}

HalManager::~HalManager()
{ }

HalManager & HalManager::getInstance()
{
	return instance;
}

int HalManager::init(GpsCallbacks * cb)
{
	(void)(cb);

	ALOGI("Initialize the HAL");

	config::read();
	
	LocServiceProxy::gps::sendSystemInfo(2017);

	ALOGI("Initialize modules");

	initUtils();
	initDevice();
	initStagps();

	ALOGI("Set capabilities");
	setCapabilites(GPS_CAPABILITY_SCHEDULING     |
	               GPS_CAPABILITY_SINGLE_SHOT    |
	               GPS_CAPABILITY_ON_DEMAND_TIME |
	               GPS_CAPABILITY_MEASUREMENTS   |
	               GPS_CAPABILITY_NAV_MESSAGES);

	LocServiceProxy::gps::getSignals().injectTime.connect(
		SlotFactory::create(utils::injectTime));

	return 0;
}

void HalManager::cleanup(void)
{
	delete stream;
	delete byteStream;
	delete decoder;
	delete device;
	stream = nullptr;
	byteStream = nullptr;
	decoder = nullptr;
	device = nullptr;
}

void HalManager::initUtils()
{
	ALOGI("Init utils");
	utils::Wakelock::acquire.connect(SlotFactory::create(LocServiceProxy::gps::acquireWakelock));
	utils::Wakelock::release.connect(SlotFactory::create(LocServiceProxy::gps::releaseWakelock));
}

void HalManager::initDevice()
{
	ALOGI("Init device");
	device = new NmeaDevice();
	decoder = new decoder::NmeaDecoder(*device);
	encoder = new protocol::NmeaEncoder();
	byteStream = new stream::UartByteStream(config::get().device.tty.c_str());
	stream = new stream::NmeaStream(*byteStream);

	stream->onNewBytes.connect(SlotFactory::create(*decoder, &decoder::AbstractDecoder::onNewBytes));

	device->sendMessage.connect(SlotFactory::create(*encoder, &protocol::IEncoder::encode));
	//encoder->encodedBytes.connect(SlotFactory::create(*byteStream, &stream::IByteStream::write));

	device->startNavigation.connect(SlotFactory::create(*decoder, &decoder::AbstractDecoder::start));
	device->startNavigation.connect(SlotFactory::create(*stream, &stream::IStream::startReading));
	
	device->stopNavigation.connect(SlotFactory::create(*decoder, &decoder::AbstractDecoder::stop));
	device->stopNavigation.connect(SlotFactory::create(*stream, &stream::IStream::stopReading));

	auto & gpsSignals = LocServiceProxy::gps::getSignals();
	gpsSignals.start.connect(SlotFactory::create(*device, &AbstractDevice::start));
	gpsSignals.stop.connect(SlotFactory::create(*device, &AbstractDevice::stop));

	device->onNmea.connect(SlotFactory::create(LocServiceProxy::gps::sendNmea));
	device->locationUpdate.connect(SlotFactory::create(LocServiceProxy::gps::sendLocationUpdate));
	device->satelliteListUpdate.connect(SlotFactory::create(LocServiceProxy::gps::sendSatelliteListUpdate));
	device->statusUpdate.connect(SlotFactory::create(LocServiceProxy::gps::sendStatusUpdate));

	device->requestUtcTime.connect(SlotFactory::create(LocServiceProxy::gps::requestUtcTime));

	device->init();
}

#ifdef STAGPS_ENABLED
void HalManager::initStagps()
{
	ALOGI("Initialize STAGPS");

	if(config::get().stagps.enable)
	{
		stm::stagps::print_version();
		stagpsEngine = new stm::stagps::StagpsEngine();

		// @TODO connect version request signal

		device->startNavigation.connect(
			SlotFactory::create(*stagpsEngine, &stagps::StagpsEngine::start));

		device->stopNavigation.connect(
			SlotFactory::create(*stagpsEngine, &stagps::StagpsEngine::stop));
	}
}
#else
void HalManager::initStagps()
{
	ALOGI("ST-AGPS not included in build");
}
#endif

} // namespace stm
