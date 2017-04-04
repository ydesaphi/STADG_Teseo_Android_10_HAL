#include "LocServiceProxy.h"

#define LOG_TAG "teseo_hal_LocServiceProxy"
#include <cutils/log.h>
#include <hardware/hardware.h>
#include <hardware/gps.h>
#include <string.h>
#include <unordered_map>

#include "HalManager.h"

namespace stm {
namespace LocServiceProxy {

namespace gps {

}

static struct {
	GpsCallbacks gps;
} callbacks;

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

const GpsInterface * getGpsInterface(struct gps_device_t * device)
{
	(void)(device);
	return &(interfaces.gps);
}

namespace gps {

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

const void * onGetExtension(const char * name)
{
	ALOGI("Get extension '%s'", name);
	return interfacesMap.count(name) == 1 ? interfacesMap[name] : NULL;
}

void sendNmea(GpsUtcTime timestamp, const NmeaMessage & nmea)
{
	std::string asString = nmea.toString();
	callbacks.gps.nmea_cb(timestamp, asString.c_str(), asString.size());
}

void sendLocationUpdate(Location & loc)
{
	ALOGI("Report location: %s", loc.toString().c_str());
	callbacks.gps.location_cb(loc.getAndroidStruct());
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

size_t getInternalState(char * buffer, size_t bufferSize)
{
	static const char data[] = "TeseoHal-debug: no data";

	strncpy(buffer, data, bufferSize);

	if(sizeof(data) > bufferSize)
		buffer[bufferSize - 1] = '\0';

	return sizeof(data);
}

} // namespace debug

} // namespace LocServiceProxy
} // namespace stm