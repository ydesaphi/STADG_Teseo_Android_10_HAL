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

	delete stream;
	delete byteStream;
	delete decoder;
	delete device;
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
	byteStream = new stream::UartByteStream(config::get().device.tty.c_str());
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

} // namespace stm
