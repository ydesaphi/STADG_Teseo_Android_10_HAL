/*
 * This file is part of Teseo Android HAL
 *
 * Copyright (c) 2016-2020, STMicroelectronics - All Rights Reserved
 * Author(s): Baudouin Feildel <baudouin.feildel@st.com> for STMicroelectronics.
 *
 * License terms: Apache 2.0.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

#include <teseo/utils/Gnss_1_1.h>
#include <teseo/utils/LegacyGps.h>

#include <teseo/utils/Signal.h>
#include <teseo/model/NmeaMessage.h>
#include <teseo/model/Location.h>
#include <teseo/model/SatInfo.h>
#include <teseo/geofencing/model.h>
#include <teseo/utils/Thread.h>

namespace stm {

/**
 * The LocServiceProxy namespace contains functions, structures and signals used to communicate
 * between the Android Location Services and the HAL.
 *
 * @details    The HAL interfaces are
 */
namespace LocServiceProxy {

/**
 * @brief      Open function called by the Android platform to create the "HAL device"
 */
void openDevice(void);

/**
 * @brief      Close function called by the Android platform to close the "HAL device"
 */
void closeDevice(void);

pthread_t createThreadCb(
    const char* name,
    void (*start)(void*),
    void* arg);

/**
 * Namespace containing interfaces and callbacks from GpsInterface and GpsCallbacks
 */
namespace gps {

	/**
	 * @brief GPS Signals received from Android Location services
	 */
	struct Signals {
		Signal<int, const sp<IGnssCallback>&> init = Signal<int, const sp<IGnssCallback>&>("gps::signals::init");

		Signal<int> start = Signal<int>("gps::signals::start");

		Signal<int> stop = Signal<int>("gps::signals::stop");

		Signal<void> cleanup = Signal<void>("gps::signals::cleanup");

		Signal<int, GnssUtcTime, int64_t, int> injectTime =
			Signal<int, GnssUtcTime, int64_t, int>("gps::signals::injectTime");

		Signal<int, double, double, float> injectLocation =
			Signal<int, double, double, float>("gps::signals::injectLocation");

		Signal<void, GnssAidingData> deleteAidingData =
			Signal<void, GnssAidingData>("gps::signals::deleteAidingData");

		Signal<int, GnssPositionMode, GnssPositionRecurrence, uint32_t, uint32_t, uint32_t> setPositionMode =
			Signal<int, GnssPositionMode, GnssPositionRecurrence, uint32_t, uint32_t, uint32_t>("gps::signals::setPositionMode");

		Signal<int, int> setGNSSConstellationMask = Signal<int, int>("gps::signals::setGNSSConstellationMask");

		Signal<int> getGNSSConstellationMask = Signal<int>("gps::signals::getGNSSConstellationMask");
	};

	int onInit(const sp<IGnssCallback>& cb);

	int onStart(void);

	int onStop(void);

	void onCleanup(void);

	int onInjectTime(GnssUtcTime time, int64_t timeReference, int uncertainty);

	int onInjectLocation(double latitude, double longitude, float accuracy);

	void onDeleteAidingData(GnssAidingData flags);

	int onSetPositionMode(GnssPositionMode mode, GnssPositionRecurrence recurrence,uint32_t minInterval,uint32_t preferredAccuracy,uint32_t preferredTime);

	int onSetGNSSConstellationMask(int mask);

	int onGetGNSSConstellationMask(void);

	bool onGetExtension(const char * name);

	/**
	 * @brief      Gets the GPS signal list
	 *
	 * @return     The GPS signal list
	 */
	Signals & getSignals();

	void sendNmea(GnssUtcTime timestamp, const NmeaMessage & nmea);

	void sendLocationUpdate(const Location & loc);

	void sendSatelliteListUpdate(const std::map<SatIdentifier, SatInfo> & satellites);

	void sendCapabilities(uint32_t capabilities);

	void sendStatusUpdate(GnssStatusValue status);

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

	void onInit(const sp<IGnssGeofenceCallback>& cb);

	void onAddGeofenceArea (int32_t geofence_id, double latitude, double longitude, double radius_meters, int last_transition, int monitor_transitions, int notification_responsiveness_ms, int unknown_timer_ms);

	void onPauseGeofence(int32_t geofence_id);

	void onResumeGeofence(int32_t geofence_id, int monitor_transitions);

	void onRemoveGeofenceArea(int32_t geofence_id);

	Signals & getSignals();

	void sendGeofenceTransition(GeofenceId geofence_id,  const Location & loc, Transition transition, GnssUtcTime timestamp);

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

//#ifdef STRAW_ENABLED
namespace navigationMessage {

	struct Signals {
		Signal<int, const sp<IGnssNavigationMessageCallback> &> init = Signal<int, const sp<IGnssNavigationMessageCallback> &>("navigationMessage::signals::init");
		Signal<void> close = Signal<void>("navigationMessage::signals::close");
	};

	Signals & getSignals();

	int onInit(const sp<IGnssNavigationMessageCallback>& cb);
	void onClose(void);

	void sendNavigationMessages(GnssNavigationMessage & msg);
}

namespace measurement {

	struct Signals {
		Signal<int, const sp<IGnssMeasurementCallback> &> init = Signal<int, const sp<IGnssMeasurementCallback> &>("measurement::signals::init");
		Signal<void> close = Signal<void>("measurement::signals::close");
	};

	Signals & getSignals();

	int onInit(const sp<IGnssMeasurementCallback>& cb);
	void onClose(void);
	void sendMeasurements(
        const GnssClock & clockData,
        std::vector <GnssMeasurement> & measurementdata);
}
//#endif

namespace ril {

	struct Signals {
		Signal<void> init = Signal<void> ("ril::signals::init");

		Signal<void, const AGnssRefLocation *> setRefLocation = Signal<void, const IAGnssRil::AGnssRefLocation *> ("ril::signals::setRefLocation");

		Signal<void, IAGnssRil::SetIDType, const char *> setSetId = Signal<void, IAGnssRil::SetIDType, const char *> ("ril::signals::setSetId");

		Signal<void, uint8_t *> niMessage = Signal<void, uint8_t *> ("ril::signals::niMessage");

		Signal<void, int, int,int> updateNetworkState = Signal<void, int, int,int> ("ril::signals::updateNetworkState");

		Signal<void, int> updateNetworkAvailability = Signal<void, int> ("ril::signals::updateNetworkAvailability");
	};

	void onInit(const sp<IAGnssRilCallback>& cb);
	void onSetRefLocation(const AGnssRefLocation *agnss_reflocation);
	void onSetSetId(SetIDType type, const char *setid);
 	void onNiMessage(uint8_t *msg, size_t len);
	void onUpdateNetworkState(int connected, int type, int roaming, const char *extra_info);
	void onUpdateNetworkAvailability(int available, const char *apn);

	Signals & getSignals();

	void sendRequestSetId(SetID flags);

	void sendRequestReferenceLocation(uint32_t flags);

} // namespace ril

namespace ni
{
	struct Signals {
		Signal<void> init = Signal<void>("ni::signals::init");
		Signal<void, int, IGnssNiCallback::GnssUserResponseType> respond = Signal<void, int, IGnssNiCallback::GnssUserResponseType>("ni::signals::respond");
	};

	void onInit(const sp<IGnssNiCallback>& cb);
	void onResponse(int notif_id, IGnssNiCallback::GnssUserResponseType user_response);
	Signals & getSignals();
	void sendNiNotificationRequest(IGnssNiCallback::GnssNiNotification &notification);

} // namespace ni

namespace agps
{

	struct Signals {
		Signal<void> init = Signal<void>("agps::signals:: init");
		Signal<int,AGnssType,const char*,int> setServer = Signal<int,AGnssType,const char*,int>("agps::signals::setServer");
	};

	Signals & getSignals();
	void onInit(const sp<IAGnssCallback>& cb);
	int onDataConnOpen(const char* apn);
	int onDataConnClosed(void);
	int onDataConnFailed(void);
	int onSetServer(AGnssType type, const char* hostname, int port);
	int onDataConnOpenWithApnType(const char* apn, ApnIpType apnIpType);

	void sendAGpsStatusIpV4(AGnssStatusIpV4 status);
	void sendAGpsStatusIpV6(AGnssStatusIpV6 status);
}

namespace configuration
{
	struct Signals {
		Signal<void,const char*,int32_t> cfgUpdateSig = Signal<void,const char*,int32_t>("configuration::signals::cfgUpdateSig");
	};

	Signals & getSignals();
	void onConfigurationUpdate(const char* config_data, int32_t length);
}

} // namespace LocServiceProxy
} // namespace stm

#endif // TESEO_HAL_LOCATION_PROXY_H
