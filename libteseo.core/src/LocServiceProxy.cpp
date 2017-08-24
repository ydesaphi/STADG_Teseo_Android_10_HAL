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
 * @brief Android Location Service proxy
 * @file LocServiceProxy.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/LocServiceProxy.h>

#define LOG_TAG "teseo_hal_LocServiceProxy"
#include <cutils/log.h>
#include <hardware/hardware.h>
#include <hardware/gps.h>
#include <string.h>
#include <unordered_map>

#include <teseo/HalManager.h>
#include <teseo/utils/Thread.h>

namespace stm {
namespace LocServiceProxy {

static struct {
	GpsCallbacks gps;
} callbacks;

static struct gps_device_t * device = nullptr;

void RegisterCallbacks(const GpsCallbacks * cb)
{
	memcpy(&(callbacks.gps), cb, sizeof(GpsCallbacks));
	Thread::setCreateThreadCb(callbacks.gps.create_thread_cb);
}

int openDevice(const struct hw_module_t * module, char const * name, struct hw_device_t ** dev)
{
	if(device == NULL) {
		device = new struct gps_device_t();
		device->common.tag         = HARDWARE_DEVICE_TAG;
		device->common.version     = 0;
		device->common.module      = (struct hw_module_t *)module;
		device->common.close       = &LocServiceProxy::closeDevice;
		device->get_gps_interface  = &LocServiceProxy::getGpsInterface;
	}

	*dev = (struct hw_device_t*)(device);

	gps::getSignals().init.connect(
		SlotFactory::create(HalManager::getInstance(), &HalManager::init));

	gps::getSignals().cleanup.connect(
		SlotFactory::create(HalManager::getInstance(), &HalManager::cleanup));

	ALOGI("New device instanciated at address %p, with name: '%s'", dev, name);

	return (device != NULL) ? 0 : -1;
}

int closeDevice(struct hw_device_t * dev)
{
	if(dev != (struct hw_device_t*)(device)) {
		ALOGW("Request to close a device not opened by Teseo HAL, closing anyway.");
		ALOGI("Close device at %p.", dev);
		delete dev;
	}

	if(device != NULL) {
		ALOGI("Close device at %p.", device);
		delete device;
		device = NULL;
	}

	return 0;
}

namespace gps {

const void * onGetExtension(const char * name);

static Signals signals;

Signals & getSignals()
{
	return signals;
}

int onInit(GpsCallbacks * cb)
{
	RegisterCallbacks(cb);
	signals.init.emit(cb);
	return 0;
}

int onStart(void)
{
	signals.start.emit();
	return 0;
}

int onStop(void)
{
	signals.stop.emit();
	return 0;
}

void onCleanup(void)
{
	signals.cleanup.emit();
}

int onInjectTime(GpsUtcTime time, int64_t timeReference, int uncertainty)
{
	signals.injectTime.emit(time, timeReference, uncertainty);
	return 0;
}

int onInjectLocation(double latitude, double longitude, float accuracy)
{
	signals.injectLocation.emit(latitude, longitude, accuracy);
	return 0;
}

void onDeleteAidingData(GpsAidingData flags)
{
	signals.deleteAidingData.emit(flags);
}

int onSetPositionMode(
	GpsPositionMode mode,
	GpsPositionRecurrence recurrence,
	uint32_t minInterval,
	uint32_t preferredAccuracy,
	uint32_t preferredTime)
{
	signals.setPositionMode.emit(mode, recurrence, minInterval, preferredAccuracy, preferredTime);
	return 0;
}

void sendNmea(GpsUtcTime timestamp, const NmeaMessage & nmea)
{
	std::string asString = nmea.toString();
	callbacks.gps.nmea_cb(timestamp, asString.c_str(), asString.size());
}

void sendStatusUpdate(GpsStatusValue value)
{
	GpsStatus status = { .size = sizeof(GpsStatus) };

	ALOGI("Send status update: %d", value);
	status.status = value;
	callbacks.gps.status_cb(&status);
}

void sendSystemInfo(uint16_t yearOfHardware)
{
	GnssSystemInfo sysInfo = {.size = sizeof(GnssSystemInfo)};

	ALOGI("Send system info (year of hardware): %d", yearOfHardware);
	sysInfo.year_of_hw = yearOfHardware;
	callbacks.gps.set_system_info_cb(&sysInfo);
}

void sendLocationUpdate(const Location & loc)
{
	GpsLocation location;
	loc.copyToGpsLocation(location);

	ALOGI("Report location: %s", loc.toString().c_str());
	callbacks.gps.location_cb(&location);
}

void sendSatelliteListUpdate(const std::map<SatIdentifier, SatInfo> & satellites)
{
	GnssSvStatus status = {.size = sizeof(GnssSvStatus), .num_svs = 0};

	GnssSvInfo * svList = status.gnss_sv_list;
	status.num_svs = satellites.size();

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

		p.second.copyToGnssSvInfo(svList);
		++svList;
	};

	if(status.num_svs < GNSS_MAX_SVS)
	{	
		std::for_each(satellites.begin(), satellites.end(), action);
	}
	else
	{
		auto it = satellites.begin();
		for(int i = 0; i < GNSS_MAX_SVS; i++) {
			action(*it);
			++it;
		}
	}

	ALOGI("Send satellite list: %d satellites, %d gps, %d glonass, %d galileo, %d beidou, %d others",
		totalSats, gpsSats, gloSats, galSats, beiSats, otherSats);
	callbacks.gps.gnss_sv_status_cb(&status);
}

void sendCapabilities(uint32_t capabilities)
{
	ALOGI("Set capabilities: 0x%x", capabilities);
	callbacks.gps.set_capabilities_cb(capabilities);
}

void acquireWakelock()
{
	callbacks.gps.acquire_wakelock_cb();
}

void releaseWakelock()
{
	callbacks.gps.release_wakelock_cb();
}

void requestUtcTime()
{
	callbacks.gps.request_utc_time_cb();
}

} // namespace gps

namespace debug {

static Signals signals;

Signals & getSignals() { return signals; }

std::size_t getInternalState(char * buffer, std::size_t bufferSize)
{
	std::string output;

	auto collectedInformation = signals.getInternalState.collect();

	if(collectedInformation.size() == 0)
	{
		output = "No data.";
	}
	else
	{
		for(auto d: collectedInformation)
		{
			output.append(d);
			output.append("\n");
		}
	}

	strncpy(buffer, output.c_str(), bufferSize);

	if(output.size() > bufferSize)
		buffer[bufferSize - 1] = '\0';

	return output.size();
}

} // namespace debug

static Interfaces interfaces = {
	.gps = {
		.size               = sizeof(GpsInterface),
		.init               = &LocServiceProxy::gps::onInit,
		.start              = &LocServiceProxy::gps::onStart,
		.stop               = &LocServiceProxy::gps::onStop,
		.cleanup            = &LocServiceProxy::gps::onCleanup,
		.inject_time        = &LocServiceProxy::gps::onInjectTime,
		.inject_location    = &LocServiceProxy::gps::onInjectLocation,
		.delete_aiding_data = &LocServiceProxy::gps::onDeleteAidingData,
		.set_position_mode  = &LocServiceProxy::gps::onSetPositionMode,
		.get_extension      = &LocServiceProxy::gps::onGetExtension
	},
	.debug = {
		.size               = sizeof(GpsDebugInterface),
		.get_internal_state = &LocServiceProxy::debug::getInternalState
	}
};

static std::unordered_map<const char *, void *> interfacesMap = {
	{GPS_XTRA_INTERFACE,               NULL}                ,
	{GPS_DEBUG_INTERFACE,              &(interfaces.debug)} ,
	{AGPS_INTERFACE,                   NULL}                ,
	{SUPL_CERTIFICATE_INTERFACE,       NULL}                ,
	{GPS_NI_INTERFACE,                 NULL}                ,
	{AGPS_RIL_INTERFACE,               NULL}                ,
	{GPS_GEOFENCING_INTERFACE,         NULL}                ,
	{GPS_MEASUREMENT_INTERFACE,        NULL}                ,
	{GPS_NAVIGATION_MESSAGE_INTERFACE, NULL}                ,
	{GNSS_CONFIGURATION_INTERFACE,     NULL}
};

const GpsInterface * getGpsInterface(struct gps_device_t * device)
{
	(void)(device);
	return &(interfaces.gps);
}

namespace gps {	
	const void * onGetExtension(const char * name)
	{
		ALOGI("Get extension '%s'", name);
		return interfacesMap.count(name) == 1 ? interfacesMap[name] : NULL;
	}
}

} // namespace LocServiceProxy
} // namespace stm