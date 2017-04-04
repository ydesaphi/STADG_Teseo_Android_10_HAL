#ifndef TESEO_HAL_LOCATION_PROXY_H
#define TESEO_HAL_LOCATION_PROXY_H

#include <cstdint>
#include <string>
#include <hardware/gps.h>

#include "Signal.h"
#include "model/NmeaMessage.h"
#include "model/Location.h"

namespace stm {
namespace LocServiceProxy {

struct Interfaces {
	GpsInterface                  gps;
	GpsXtraInterface              xtra;
	GpsDebugInterface             debug;
	AGpsInterface                 agps;
	SuplCertificateInterface      suplCertificate;
	GpsNiInterface                ni;
	AGpsRilInterface              ril;
	GpsGeofencingInterface        geofencing;
	GpsMeasurementInterface       measurement;
	GpsNavigationMessageInterface navigationMessage;
	GnssConfigurationInterface    configuration;
};

/**
 * @brief      Register GpsCallbacks
 *
 * @param[in]  cb    The GpsCallbacks to register
 */
void RegisterCallbacks(const GpsCallbacks * cb);

int openDevice(const struct hw_module_t * module, char const * name, struct hw_device_t ** dev);

int closeDevice(struct hw_device_t * dev);

const GpsInterface * getGpsInterface(struct gps_device_t * device);

/**
 * Namespace containing interfaces and callbacks from GpsInterface and GpsCallbacks
 */
namespace gps {

	/**
	 * @brief GPS Signals received from Android Location services
	 */
	struct Signals {
		Signal<int, GpsCallbacks *> init = Signal<int, GpsCallbacks *>("gps::signals::init");

		Signal<int> start = Signal<int>("gps::signals::start");

		Signal<int> stop = Signal<int>("gps::signals::stop");

		Signal<void> cleanup = Signal<void>("gps::signals::cleanup");

		Signal<int, GpsUtcTime, int64_t, int> injectTime =
			Signal<int, GpsUtcTime, int64_t, int>("gps::signals::injectTime");

		Signal<int, double, double, float> injectLocation =
			Signal<int, double, double, float>("gps::signals::injectLocation");

		Signal<void, GpsAidingData> deleteAidingData =
			Signal<void, GpsAidingData>("gps::signals::deleteAidingData");

		Signal<int, GpsPositionMode, GpsPositionRecurrence, uint32_t, uint32_t, uint32_t> setPositionMode =
			Signal<int, GpsPositionMode, GpsPositionRecurrence, uint32_t, uint32_t, uint32_t>("gps::signals::setPositionMode");
	};

	/**
	 * @brief      Gets the GPS signal list
	 *
	 * @return     The GPS signal list
	 */
	Signals & getSignals();

	void sendNmea(GpsUtcTime timestamp, const NmeaMessage & nmea);

	void sendLocationUpdate(Location & loc);

	void sendCapabilities(uint32_t capabilities);

	void acquireWakelock();

	void releaseWakelock();

	void requestUtcTime();

	int onInit(GpsCallbacks * cb);

	int onStart(void);

	int onStop(void);

	void onCleanup(void);

	int onInjectTime(GpsUtcTime time, int64_t timeReference, int uncertainty);

	int onInjectLocation(double latitude, double longitude, float accuracy);

	void onDeleteAidingData(GpsAidingData flags);

	int onSetPositionMode(
		GpsPositionMode mode,
		GpsPositionRecurrence recurrence,
		uint32_t minInterval,
		uint32_t preferredAccuracy,
		uint32_t preferredTime);

	const void * onGetExtension(const char * name);
} // namespace gps

namespace debug {
	/**
	 * @brief      Dump HAL internal state into buffer
	 *
	 * @param      buffer      The buffer
	 * @param[in]  bufferSize  The buffer size
	 *
	 * @return     Number of byte dumped
	 * 
	 * @todo Currently dummy data is dumped
	 */
	size_t getInternalState(char * buffer, size_t bufferSize);
}

} // namespace LocServiceProxy
} // namespace stm

#endif // TESEO_HAL_LOCATION_PROXY_H