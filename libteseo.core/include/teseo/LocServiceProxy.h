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
 * @file LocServiceProxy.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_LOCATION_PROXY_H
#define TESEO_HAL_LOCATION_PROXY_H

#include <cstdint>
#include <string>
#include <map>
#include <hardware/gps.h>

#include <teseo/utils/Signal.h>
#include <teseo/model/NmeaMessage.h>
#include <teseo/model/Location.h>
#include <teseo/model/SatInfo.h>
#include <teseo/geofencing/model.h>

namespace stm {

/**
 * The LocServiceProxy namespace contains functions, structures and signals used to communicate
 * between the Android Location Services and the HAL.
 *
 * @details    The HAL interfaces are
 */
namespace LocServiceProxy {

/**
 * @brief      HAL public interfaces
 */
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

#ifdef STRAW_ENABLED
/**
 * @brief      Register MeasurementsCallbacks
 *
 * @param[in]  cb    The MeasurementsCallbacks to register
 */
void RegisterMeasurementsCallbacks(const GpsMeasurementCallbacks * cb);
/**
 * @brief      Register NavigationMessageCallbacks
 *
 * @param[in]  cb    The NavigationMessageCallbacks to register
 */
 void RegisterNavigationMessageCallbacks(const GpsNavigationMessageCallbacks * cb);
#endif

/**
 * @brief      Register GpsGeofenceCallbacks
 *
 * @param[in]  cb    The GpsGeofenceCallbacks to register
 */
void RegisterGeofenceCallbacks(const GpsGeofenceCallbacks * cb);

/**
 * @brief      Register AGpsRilCallbacks
 *
 * @param[in]  cb    The AGpsRilCallbacks to register
 */
void RegisterRilCallbacks(const AGpsRilCallbacks *cb);

/**
 * @brief      Register AGpsRilCallbacks
 *
 * @param[in]  cb    The AGpsRilCallbacks to register
 */
void RegisterNiCallbacks(const GpsNiCallbacks *cb);

/**
 * @brief      Open function called by the Android platform to create the "HAL device"
 *
 * @param[in]  module  The GPS module instance
 * @param      name    The GPS module name
 * @param      dev     The pointer to device where to store the allocated device
 *
 * @return     0 on success, 1 on failure
 */
int openDevice(const struct hw_module_t * module, char const * name, struct hw_device_t ** dev);

/**
 * @brief      Close function called by the Android platform to close the "HAL device"
 *
 * @param      dev   The device to close
 *
 * @return     0 on succes, 1 on failure
 */
int closeDevice(struct hw_device_t * dev);

/**
 * @brief      Function called by the Android platform to get the HAL Gps Interface
 *
 * @param      device  The HAL device
 *
 * @return     The gps interface.
 */
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

	int onStart(void);

	int onStop(void);

	void onCleanup(void);

	int onInjectTime(GpsUtcTime time, int64_t timeReference, int uncertainty);

	int onInjectLocation(double latitude, double longitude, float accuracy);

	void onDeleteAidingData(GpsAidingData flags);

	int onSetPositionMode(GpsPositionMode mode,	GpsPositionRecurrence recurrence,uint32_t minInterval,uint32_t preferredAccuracy,uint32_t preferredTime);

	const void * onGetExtension(const char * name);

	/**
	 * @brief      Gets the GPS signal list
	 *
	 * @return     The GPS signal list
	 */
	Signals & getSignals();

	void sendNmea(GpsUtcTime timestamp, const NmeaMessage & nmea);

	void sendLocationUpdate(const Location & loc);

	void sendSatelliteListUpdate(const std::map<SatIdentifier, SatInfo> & satellites);

	void sendCapabilities(uint32_t capabilities);

	void sendStatusUpdate(GpsStatusValue status);

	void sendSystemInfo(uint16_t yearOfHardware);

	void acquireWakelock();

	void releaseWakelock();

	void requestUtcTime();

} // namespace gps

namespace geofencing {

	// Import only needed names to avoid too much writing
	using stm::geofencing::model::GeofenceDefinition;
	using stm::geofencing::model::GeofenceId;
	using stm::geofencing::model::Transition;
	using stm::geofencing::model::TransitionFlags;
	using stm::geofencing::model::SystemStatus;
	using stm::geofencing::model::OperationStatus;

	struct Signals {
		//Signal<void, GpsGeofenceCallbacks *> init = Signal<void, GpsGeofenceCallbacks *>("geofencing::signals::init");
		Signal<void> init = Signal<void>("geofencing::signals::init");

		Signal<void, GeofenceDefinition > addGeofenceArea = Signal<void, GeofenceDefinition >("geofencing::signals::addGeofenceArea");

		Signal<void, GeofenceId> pauseGeofence = Signal<void, GeofenceId>("geofencing::signals::pauseGeofence");

		Signal<void, GeofenceId, TransitionFlags> resumeGeofence = Signal<void, GeofenceId, TransitionFlags>("geofencing::signals::resumeGeofence");

		Signal<void, GeofenceId> removeGeofenceArea = Signal<void, GeofenceId>("geofencing::signals::removeGeofenceArea");
	};

	void onInit(GpsGeofenceCallbacks * callbacks);

	void onAddGeofenceArea (int32_t geofence_id, double latitude, double longitude, double radius_meters, int last_transition, int monitor_transitions, int notification_responsiveness_ms, int unknown_timer_ms);

	void onPauseGeofence(int32_t geofence_id);

	void onResumeGeofence(int32_t geofence_id, int monitor_transitions);

	void onRemoveGeofenceArea(int32_t geofence_id);

	Signals & getSignals();

	void sendGeofenceTransition(GeofenceId geofence_id,  const Location & loc, Transition transition, GpsUtcTime timestamp);

	void sendGeofenceStatus(SystemStatus status, const Location & last_location);

	void answerGeofenceAddRequest(GeofenceId geofence_id, OperationStatus status);

	void answerGeofenceRemoveRequest(GeofenceId geofence_id, OperationStatus status);

	void answerGeofencePauseRequest(GeofenceId geofence_id, OperationStatus status);

	void answerGeofenceResumeRequest(GeofenceId geofence_id, OperationStatus status);

} // namespace geofencing

namespace debug {

	struct Signals {
		Signal<std::string> getInternalState = Signal<std::string>("debug::signals::getInternalState");
	};

	Signals & getSignals();

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

#ifdef STRAW_ENABLED
namespace navigationMessage {

	struct Signals {
		Signal<int, GpsNavigationMessageCallbacks *> init = Signal<int, GpsNavigationMessageCallbacks *>("navigationMessage::signals::init");
		Signal<void> close = Signal<void>("navigationMessage::signals::close");
	};

	Signals & getSignals();

	void sendNavigationMessages(GnssNavigationMessage & msg);
}

namespace measurement {

	struct Signals {
		Signal<int, GpsMeasurementCallbacks *> init = Signal<int, GpsMeasurementCallbacks *>("measurement::signals::init");
		Signal<void> close = Signal<void>("measurement::signals::close");
	};

	Signals & getSignals();

	void sendMeasurements(const GnssClock & clockData,std::vector <GnssMeasurement> & measurementdata);
}
#endif
namespace ril {

	struct Signals {
		Signal<void> init = Signal<void>("ril::signals::init");

		Signal<void, const AGpsRefLocation *> setRefLocation = Signal<void, const AGpsRefLocation * >("ril::signals::setRefLocation");
		
		Signal<void, AGpsSetIDType, const char *> setSetId = Signal<void, AGpsSetIDType, const char *>("ril::signals::setSetId");
		
		Signal<void, uint8_t *> niMessage = Signal<void, uint8_t *>("ril::signals::niMessage");
		
		Signal<void, int, int,int>updateNetworkState = Signal<void, int, int,int>("ril::signals::updateNetworkState");

		Signal<void, int>updateNetworkAvailability = Signal<void, int>("ril::signals::updateNetworkAvailability");
	};

	void onInit(AGpsRilCallbacks *callbacks);
	void onSetRefLocation(const AGpsRefLocation *agps_reflocation, size_t sz_struct);
 	void onSetSetId( AGpsSetIDType type, const char *setid);
 	void onNiMessage(uint8_t *msg, size_t len);
	void onUpdateNetworkState(int connected, int type, int roaming, const char *extra_info);
	void onUpdateNetworkAvailability(int available, const char *apn);

	Signals & getSignals();

	void sendRequestSetId(uint32_t flags);
	void sendRequestReferenceLocation(uint32_t flags);

} // namespace ril 

namespace ni
{
	struct Signals {
		Signal<void> init = Signal<void>("ni::signals::init");
		Signal<void, int, GpsUserResponseType> respond = Signal<void, int, GpsUserResponseType>("ni::signals::respond");
	};
	void onInit(GpsNiCallbacks *callbacks);
	void onResponse(int notif_id, GpsUserResponseType user_response);
	Signals & getSignals();
	void sendNiNotificationRequest(GpsNiNotification *notification);

} // namespace ni

namespace agps
{

	struct Signals {
		Signal<void> init = Signal<void>("agps::signals:: init");
		Signal<int,AGpsType,const char*,int> setServer = Signal<int,AGpsType,const char*,int>("agps::signals::setServer");
	};

	Signals & getSignals();
	void onInit(AGpsCallbacks *callbacks);
	int onDataConnOpen(const char* apn);
	int onDataConnClosed(void);
	int onDataConnFailed(void);
	int onSetServer(AGpsType type, const char* hostname, int port);
	int onDataConnOpenWithApnType(const char* apn, ApnIpType apnIpType);

	void sendAGpsStatus(AGpsStatus* status);
}

} // namespace LocServiceProxy
} // namespace stm

#endif // TESEO_HAL_LOCATION_PROXY_H
