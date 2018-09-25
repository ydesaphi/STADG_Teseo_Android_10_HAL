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
#include <teseo/utils/http.h>

#include <teseo/utils/IByteStream.h>
#include <teseo/utils/IStream.h>
#include <teseo/device/AbstractDevice.h>
#include <teseo/protocol/AbstractDecoder.h>

#include <teseo/utils/UartByteStream.h>
#include <teseo/utils/NmeaStream.h>
#include <teseo/protocol/NmeaDecoder.h>
#include <teseo/device/NmeaDevice.h>
#include <teseo/protocol/NmeaEncoder.h>
#include <teseo/geofencing/manager.h>

#include <teseo/LocServiceProxy.h>

#ifdef STAGPS_ENABLED
#include <teseo/stagps/stagps.h>
#endif

#ifdef STRAW_ENABLED
#include <teseo/libstraw/straw.h>
#endif

#ifdef AGPS_ENABLED
#include <teseo/agnss/agps_if.h>
#include <teseo/agnss/ril_if.h>
#include <teseo/agnss/ni_if.h>
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

	config::read();
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

	//config::read();
	
	LocServiceProxy::gps::sendSystemInfo(2018);

	ALOGI("Initialize modules");

	initUtils();
	initDevice();
	initStagps();
	initGeofencing();
	initRawMeasurement();
	initAGpsIf();
	initRilIf();
	initNiIf();
	
	ALOGI("Set capabilities");
	setCapabilites(GPS_CAPABILITY_SCHEDULING     |
	#ifdef SUPL_ENABLED
				   GPS_CAPABILITY_MSB            | //MS Based
	#endif
	               GPS_CAPABILITY_SINGLE_SHOT    |
	               GPS_CAPABILITY_ON_DEMAND_TIME |
				   GPS_CAPABILITY_GEOFENCING     |
	               GPS_CAPABILITY_MEASUREMENTS   |
	               GPS_CAPABILITY_NAV_MESSAGES);

	LocServiceProxy::gps::getSignals().injectTime.connect(
		SlotFactory::create(utils::injectTime));

	return 0;
}

void HalManager::cleanup(void)
{
#ifdef STAGPS_ENABLED
	if(config::get().stagps.enable)
	{
		stagpsEngine->cleanup();
		delete stagpsEngine;
		stagpsEngine = nullptr;
	}
	else
	{
		ALOGV("Do not cleanup STAGPS engine, it is disabled");
	}
#else
	ALOGD("STAGPS Engine is not compiled, do not cleanup");
#endif

#ifdef AGPS_ENABLED
	if(config::get().agnss.enable)
	{
		delete rilIf;
		delete niIf;
		delete AgpsIf;
		rilIf = nullptr;
		niIf = nullptr;
		AgpsIf = nullptr;
	}
#else
    ALOGD("AGPS is not compiled, do not cleanup");
#endif

	delete geofencingManager;

#ifdef STRAW_ENABLED
	delete rawMeasurement;
	rawMeasurement = nullptr;
#endif

	delete stream;
	delete byteStream;
	delete decoder;
	delete device;

	geofencingManager = nullptr;
	stream = nullptr;
	byteStream = nullptr;
	decoder = nullptr;
	device = nullptr;

	utils::http_cleanup();
}

void HalManager::initUtils()
{
	ALOGI("Init utils");
	utils::Wakelock::acquire.connect(SlotFactory::create(LocServiceProxy::gps::acquireWakelock));
	utils::Wakelock::release.connect(SlotFactory::create(LocServiceProxy::gps::releaseWakelock));

	utils::http_init();
}

void HalManager::initDevice()
{
	ALOGI("Init device");
	device = new NmeaDevice();
	decoder = new decoder::NmeaDecoder(*device);
	encoder = new protocol::NmeaEncoder();
	byteStream = new stream::UartByteStream(config::get().device.tty, config::get().device.speed);
	stream = new stream::NmeaStream();

	// Bytes read stream
	// teseo -> byte stream -> nmea stream -> decoder -> device
	byteStream->newBytes.connect(SlotFactory::create(*stream, &stream::IStream::onNewBytes));
	stream->newSentence.connect(SlotFactory::create(*decoder, &decoder::AbstractDecoder::onNewBytes));

	// Bytes write stream
	// device -> encoder -> nmea stream -> byte stream -> teseo
	device->sendMessage.connect(SlotFactory::create(*encoder, &protocol::IEncoder::encode));
	encoder->encodedBytes.connect(SlotFactory::create(*stream, &stream::IStream::write));
	stream->newBytesToWrite.connect(SlotFactory::create(*byteStream, &stream::IByteStream::write));

	// Start navigation signal
	device->startNavigation.connect(SlotFactory::create(*decoder, &decoder::AbstractDecoder::start));
	device->startNavigation.connect(SlotFactory::create(*byteStream, &stream::IByteStream::start));

	// Stop navigation signal
	device->stopNavigation.connect(SlotFactory::create(*decoder, &decoder::AbstractDecoder::stop));
	device->stopNavigation.connect(SlotFactory::create(*byteStream, &stream::IByteStream::stop));

	// Data model updates
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
	if(config::get().stagps.enable)
	{
		ALOGI("Initialize STAGPS");

		stm::stagps::print_version();
		stagpsEngine = new stm::stagps::StagpsEngine();

		device->startNavigation.connect(
			SlotFactory::create(*stagpsEngine, &stagps::StagpsEngine::onStartNavigation));

		device->stopNavigation.connect(
			SlotFactory::create(*stagpsEngine, &stagps::StagpsEngine::onStopNavigation));

		device->onVersionNumber.connect(
			SlotFactory::create(*stagpsEngine, &stagps::StagpsEngine::onNewVersionAvailable));

		device->onStagps8Answer.connect(
			SlotFactory::create(*stagpsEngine, &stagps::StagpsEngine::onStagps8Answer));

		device->onStagpsAnswer.connect(
			SlotFactory::create(*stagpsEngine, &stagps::StagpsEngine::onStagpsAnswer));

		stagpsEngine->sendMessageRequest.connect(
			SlotFactory::create(*device, &device::AbstractDevice::sendMessageRequest));

		device->locationUpdate.connect(
			SlotFactory::create(*stagpsEngine, &stagps::StagpsEngine::onLocationAvailable));

		stagpsEngine->init();
	}
	else
	{
		ALOGI("STAGPS Disabled in configuration");
	}
}
#else
void HalManager::initStagps()
{
	ALOGI("ST-AGPS not included in build");
}
#endif

void HalManager::initGeofencing()
{
	using namespace stm::geofencing;

	ALOGI("Initialize Geofencing");

	geofencingManager = new GeofencingManager();

	geofencingManager->answerGeofenceAddRequest.connect(SlotFactory::create(LocServiceProxy::geofencing::answerGeofenceAddRequest));
	geofencingManager->answerGeofenceRemoveRequest.connect(SlotFactory::create(LocServiceProxy::geofencing::answerGeofenceRemoveRequest));
	geofencingManager->answerGeofencePauseRequest.connect(SlotFactory::create(LocServiceProxy::geofencing::answerGeofencePauseRequest));
	geofencingManager->answerGeofenceResumeRequest.connect(SlotFactory::create(LocServiceProxy::geofencing::answerGeofenceResumeRequest));

	geofencingManager->sendGeofenceStatus.connect(SlotFactory::create(LocServiceProxy::geofencing::sendGeofenceStatus));
	geofencingManager->sendGeofenceTransition.connect(SlotFactory::create(LocServiceProxy::geofencing::sendGeofenceTransition));

	auto & geofencingSignals = LocServiceProxy::geofencing::getSignals();
	// Initialize slot is empty
	geofencingSignals.init.connect(SlotFactory::create(*geofencingManager, &GeofencingManager::initialize));
	geofencingSignals.addGeofenceArea.connect(SlotFactory::create(*geofencingManager, &GeofencingManager::add));
	geofencingSignals.removeGeofenceArea.connect(SlotFactory::create(*geofencingManager, &GeofencingManager::remove));
	geofencingSignals.pauseGeofence.connect(SlotFactory::create(*geofencingManager, &GeofencingManager::pause));
	geofencingSignals.resumeGeofence.connect(SlotFactory::create(*geofencingManager, &GeofencingManager::resume));

	device->locationUpdate.connect(SlotFactory::create(*geofencingManager, &GeofencingManager::onLocationUpdate));
	device->statusUpdate.connect(SlotFactory::create(*geofencingManager, &GeofencingManager::onDeviceStatusUpdate));
}

#ifdef STRAW_ENABLED
void HalManager::initRawMeasurement(void)
{
	using namespace stm::straw;

	ALOGI("Initialize  HalManager::initRawMeasurement");

	rawMeasurement = new StrawEngine();

	auto & gnssSignals = LocServiceProxy::measurement::getSignals();
	gnssSignals.init.connect(SlotFactory::create(*rawMeasurement, &StrawEngine::initMeasurement));
	gnssSignals.close.connect(SlotFactory::create(*rawMeasurement, &StrawEngine::closeMeasurement));

	rawMeasurement->sendMeasurements.connect(SlotFactory::create(LocServiceProxy::measurement::sendMeasurements));
	rawMeasurement->sendNavigathionMessages.connect(SlotFactory::create(LocServiceProxy::navigationMessage::sendNavigationMessages));

	device->onNmea.connect(SlotFactory::create(*rawMeasurement, &StrawEngine::onNmeaMessage));

	auto & navSignals = LocServiceProxy::navigationMessage::getSignals();
	navSignals.init.connect(SlotFactory::create(*rawMeasurement, &StrawEngine::initNavigationMessages));
	navSignals.close.connect(SlotFactory::create(*rawMeasurement, &StrawEngine::closeNavigationMessages));
}
#else
void HalManager::initRawMeasurement(void)
{
	ALOGI("ST-RAW not included in build");
}
#endif

#ifdef AGPS_ENABLED
void HalManager::initRilIf()
{
	//using namespace stm::ril;
	if(config::get().agnss.enable)
	{
		ALOGI("Initialize RIL interface");	

		rilIf = new ril::Ril_If();
	
		// Ril interface -> framework signals
		rilIf->reqRefLoc.connect(SlotFactory::create(LocServiceProxy::ril::sendRequestReferenceLocation));
		rilIf->reqSetId.connect(SlotFactory::create(LocServiceProxy::ril::sendRequestSetId));

		//Framework -> ril interface signals
		auto & rilSignals = LocServiceProxy::ril::getSignals();

		rilSignals.init.connect(SlotFactory::create(*rilIf, &ril::Ril_If::initialize));
		rilSignals.setRefLocation.connect(SlotFactory::create(*rilIf, &ril::Ril_If::setRefLocation));
		rilSignals.setSetId.connect(SlotFactory::create(*rilIf, &ril::Ril_If::setSetId));
		rilSignals.niMessage.connect(SlotFactory::create(*rilIf, &ril::Ril_If::niMessage));
		rilSignals.updateNetworkState.connect(SlotFactory::create(*rilIf, &ril::Ril_If::updateNetworkState));
		rilSignals.updateNetworkAvailability.connect(SlotFactory::create(*rilIf, &ril::Ril_If::updateNetworkAvailability));
	}
}
#else
void HalManager::initRilIf()
{
	ALOGI("Ril IF not included in build");
}
#endif // 	#ifdef AGPS_ENABLED

#ifdef AGPS_ENABLED
void HalManager::initNiIf(){
	if(config::get().agnss.enable)
	{
		ALOGI("Initialize NI interface");	

		niIf = new ni::Ni_If();
		auto & niSignals = LocServiceProxy::ni::getSignals();
		// Ni interface -> framework signals
		niIf->reqNiNotification.connect(SlotFactory::create(LocServiceProxy::ni::sendNiNotificationRequest));

		//Framework -> ni interface signals
		niSignals.init.connect(SlotFactory::create(*niIf, &ni::Ni_If::initialize));
		niSignals.respond.connect(SlotFactory::create(*niIf, &ni::Ni_If::respond));
	}
}
#else
void HalManager::initNiIf(){
	ALOGI("Ni IF not included in build");
}
#endif //#ifdef AGPS_ENABLED

#ifdef AGPS_ENABLED
void HalManager::initAGpsIf()
{
	if(config::get().agnss.enable)
	{
		AgpsIf = new agps::Agps_If();

		auto & agpsSignals = LocServiceProxy::agps::getSignals();
		agpsSignals.init.connect(SlotFactory::create(*AgpsIf, &agps::Agps_If::initialize));
		agpsSignals.setServer.connect(SlotFactory::create(*AgpsIf, &agps::Agps_If::setServer));

		AgpsIf->statusCb.connect(SlotFactory::create(LocServiceProxy::agps::sendAGpsStatus));
	}
}
#else
void HalManager::initAGpsIf()
{
	ALOGI("AGPS IF not included in build");
}
#endif //#ifdef AGPS_ENABLED
} // namespace stm
