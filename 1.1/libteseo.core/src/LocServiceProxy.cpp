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
 * @file LocServiceProxy.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#define LOG_TAG "teseo_hal_LocServiceProxy"
#include <log/log.h>

#include <teseo/LocServiceProxy.h>

#include <string.h>
#include <unordered_map>
#include <string_view>
#include <teseo/config/config.h>
#include <teseo/config/configuration_if.h>
#include <teseo/model/GpsState.h>
#include <teseo/HalManager.h>

namespace stm {
namespace LocServiceProxy {

sp<IGnssCallback> sGnssCallback = nullptr;
std::vector<std::unique_ptr<Thread::ThreadFuncArgs>> sThreadFuncArgsList;

sp<IAGnssCallback> sAGnssCallback = nullptr;
sp<IAGnssRilCallback> sAGnssRilCallback = nullptr;
sp<IGnssNiCallback> sGnssNiCallback = nullptr;
sp<IGnssMeasurementCallback> sGnssMeasureCallback = nullptr;
sp<IGnssGeofenceCallback> sGnssGeofenceCallback = nullptr;
sp<IGnssNavigationMessageCallback> sGnssNavigationMsgCallback = nullptr;

pthread_t createThreadCb(const char* name, void (*start)(void*), void* arg) {

    return Thread::createPthread(name, start, arg, &sThreadFuncArgsList);
}

void openDevice(void)
{

	gps::getSignals().init.connect(
		SlotFactory::create(HalManager::getInstance(), &HalManager::init));

	gps::getSignals().cleanup.connect(
		SlotFactory::create(HalManager::getInstance(), &HalManager::cleanup));

	stm::config::Config_If * CfgIf = stm::config::Config_If::getInstance();

	configuration::getSignals().cfgUpdateSig.connect(
		SlotFactory::create(*CfgIf, &stm::config::Config_If::configuration_update));

	ALOGI("New device instanciated");
}

void closeDevice(void)
{
	ALOGI("Close device");
}

namespace gps {

static Signals signals;

Signals & getSignals()
{
	return signals;
}

int onInit(const sp<IGnssCallback>& cb)
{
	sGnssCallback = cb;
	signals.init.emit(sGnssCallback);
	Thread::setCreateThreadCb(createThreadCb);

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

int onInjectTime(GnssUtcTime time, int64_t timeReference, int uncertainty)
{
	signals.injectTime.emit(time, timeReference, uncertainty);
	return 0;
}

int onInjectLocation(double latitude, double longitude, float accuracy)
{
	signals.injectLocation.emit(latitude, longitude, accuracy);
	return 0;
}

void onDeleteAidingData(GnssAidingData flags)
{
	signals.deleteAidingData.emit(flags);
}

int onSetPositionMode(
	GnssPositionMode mode,
	GnssPositionRecurrence recurrence,
	uint32_t minInterval,
	uint32_t preferredAccuracy,
	uint32_t preferredTime)
{
	//signals.setPositionMode.emit(mode, recurrence, minInterval, preferredAccuracy, preferredTime);
	stm::GpsState *GpsStateInst = GpsState::getInstance();
	GpsStateInst->SetPositionMode(mode, recurrence, minInterval, preferredAccuracy, preferredTime);
	return 0;
}

int onSetGNSSConstellationMask(int mask)
{
	signals.setGNSSConstellationMask.emit(mask);
	return 0;
}

int onGetGNSSConstellationMask(void)
{
	signals.getGNSSConstellationMask.emit();
	return 0;
}

void sendNmea(GnssUtcTime timestamp, const NmeaMessage & nmea)
{
	std::string asString = nmea.toString();

	android::hardware::hidl_string nmeaString;
	nmeaString.setToExternal(asString.c_str(), asString.size());

	sGnssCallback->gnssNmeaCb(timestamp, nmeaString);
}

void sendStatusUpdate(GnssStatusValue status)
{
	ALOGI("Send status update: %hhu", status);
	sGnssCallback->gnssStatusCb(status);
}

void sendSystemInfo(uint16_t yearOfHardware)
{
	GnssSystemInfo sysInfo;
	sysInfo.yearOfHw = yearOfHardware;

	ALOGI("Send system info (year of hardware): %d", yearOfHardware);
	sGnssCallback->gnssSetSystemInfoCb(sysInfo);
}

void sendLocationUpdate(const Location & loc)
{
	GnssLocation location;
	loc.copyToGnssLocation(location);

	ALOGI("Report location: %s", loc.toString().c_str());
	sGnssCallback->gnssLocationCb(location);
}

void sendSatelliteListUpdate(const std::map<SatIdentifier, SatInfo> & satellites)
{
	GnssSvStatus svStatus;
	GnssSvInfo svInfo;

	svStatus.numSvs = satellites.size();

	int gpsSats = 0;
	int sbaSats = 0;
	int gloSats = 0;
	int qzsSats = 0;
	int beiSats = 0;
	int galSats = 0;
	int otherSats = 0;
	int totalSats = 0;

    auto action = [&] (auto & p) {
        p.second.copyToGnssSvInfo(&svInfo);
        svStatus.gnssSvList[totalSats] = svInfo;

        switch(p.second.getId().getConstellation()) {
            case GnssConstellationType::GPS:
                gpsSats++;
                break;
            case GnssConstellationType::SBAS:
                sbaSats++;
                break;
            case GnssConstellationType::GLONASS:
                gloSats++;
                break;
            case GnssConstellationType::QZSS:
                qzsSats++;
                break;
            case GnssConstellationType::BEIDOU:
                beiSats++;
                break;
            case GnssConstellationType::GALILEO:
                galSats++;
                break;
            default:
                otherSats++;
                break;
        }
        totalSats++;
    };

	if(satellites.size() < static_cast<uint32_t>(GnssMax::SVS_COUNT))
	{
		std::for_each(satellites.begin(), satellites.end(), action);
	}
	else
	{
		auto it = satellites.begin();
		for(uint32_t i = 0; i < static_cast<uint32_t>(GnssMax::SVS_COUNT); i++) {
			action(*it);
			++it;
		}
	}

	ALOGI("Send satellite list: %d satellites, %d gps, %d sbas, %d glonass, %d qzss, %d beidou, %d galileo, %d others",
		totalSats,
        gpsSats,
        sbaSats,
        gloSats,
        qzsSats,
        beiSats,
        galSats,
        otherSats);
	sGnssCallback->gnssSvStatusCb(svStatus);
}

void sendCapabilities(uint32_t capabilities)
{
	ALOGI("Set capabilities: 0x%x", capabilities);
	sGnssCallback->gnssSetCapabilitesCb(capabilities);
}

void acquireWakelock()
{
	sGnssCallback->gnssAcquireWakelockCb();
}

void releaseWakelock()
{
	sGnssCallback->gnssReleaseWakelockCb();
}

void requestUtcTime()
{
	sGnssCallback->gnssRequestTimeCb();
}

} // namespace gps

namespace geofencing {

using namespace stm::geofencing::model;

static Signals signals;

Signals & getSignals() { return signals; }

void onInit(const sp<IGnssGeofenceCallback>& cb)
{
	ALOGI("Initialize geofence system request");
	sGnssGeofenceCallback = cb;
	signals.init();
}

void onAddGeofenceArea (int32_t geofence_id, double latitude, double longitude, double radius_meters, int last_transition, int monitor_transitions, int notification_responsiveness_ms, int unknown_timer_ms)
{
	ALOGI("Add geofence request");
	GeofenceDefinition def;
	def.id = geofence_id;
	def.origin.latitude = DecimalDegreeCoordinate(latitude);
	def.origin.longitude = DecimalDegreeCoordinate(longitude);
	def.radius = radius_meters;
	def.last_transition = static_cast<Transition>(last_transition);
	def.monitor_transitions = monitor_transitions;
	def.notifications_responsiveness = std::chrono::milliseconds(notification_responsiveness_ms);
	def.unknown_time = std::chrono::milliseconds(unknown_timer_ms);

	signals.addGeofenceArea(std::move(def));
}

void onPauseGeofence(int32_t geofence_id)
{
	ALOGI("Pause geofence request");
	signals.pauseGeofence(geofence_id);
}

void onResumeGeofence(int32_t geofence_id, int monitor_transitions)
{
	ALOGI("Resume geofence request");
	signals.resumeGeofence(geofence_id, monitor_transitions);
}

void onRemoveGeofenceArea(int32_t geofence_id)
{
	ALOGI("Remove geofence request");
	signals.removeGeofenceArea(geofence_id);
}

void sendGeofenceTransition(GeofenceId geofence_id,  const Location & loc, Transition transition, GnssUtcTime timestamp)
{
	// Convert location to Android location format
	GnssLocation location;
	loc.copyToGnssLocation(location);

	ALOGI("Send geofence transition: id = %d, loc = %s",
        geofence_id,
        loc.toString().c_str());

	sGnssGeofenceCallback->gnssGeofenceTransitionCb(
        geofence_id,
        location,
        static_cast<GeofenceTransition>(transition),
        timestamp);
}

void sendGeofenceStatus(SystemStatus status, const Location & last_location)
{
	GnssLocation location;

	last_location.copyToGnssLocation(location);

	ALOGI("Send geofence system status: %d", static_cast<int32_t>(status));

	sGnssGeofenceCallback->gnssGeofenceStatusCb(
        static_cast<GeofenceAvailability>(status),
        location);
}

void answerGeofenceAddRequest(GeofenceId geofence_id, OperationStatus status)
{
	ALOGI("Answer geofence add request; id=%d, result=%d", geofence_id, static_cast<int32_t>(status));

	sGnssGeofenceCallback->gnssGeofenceAddCb(
        geofence_id,
        static_cast<GeofenceStatus>(status));
}

void answerGeofenceRemoveRequest(GeofenceId geofence_id, OperationStatus status)
{
	ALOGI("Answer geofence remove request; id=%d, result=%d", geofence_id, static_cast<int32_t>(status));

	sGnssGeofenceCallback->gnssGeofenceRemoveCb(
        geofence_id,
        static_cast<GeofenceStatus>(status));
}

void answerGeofencePauseRequest(GeofenceId geofence_id, OperationStatus status)
{
	ALOGI("Answer geofence pause request; id=%d, result=%d", geofence_id, static_cast<int32_t>(status));

	sGnssGeofenceCallback->gnssGeofencePauseCb(
        geofence_id,
        static_cast<GeofenceStatus>(status));
}

void answerGeofenceResumeRequest(GeofenceId geofence_id, OperationStatus status)
{
	ALOGI("Answer geofence resume request; id=%d, result=%d", geofence_id, static_cast<int32_t>(status));

	sGnssGeofenceCallback->gnssGeofenceResumeCb(
        geofence_id,
        static_cast<GeofenceStatus>(status));
}

} // namespace geofencing

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

namespace measurement {

static Signals signals;

Signals & getSignals()
{
	return signals;
}

#ifdef STRAW_ENABLED
int onInit(const sp<IGnssMeasurementCallback>& cb)
{
	sGnssMeasureCallback = cb;
	signals.init.emit(sGnssMeasureCallback);

	return 0;
}

void onClose(void)
{
	signals.close.emit();
}

void sendMeasurements(
    const GnssClock & clockData,
    std::vector<GnssMeasurement>& measurementsData)
{
	ALOGI("SendMeasurements");

	GnssData gnssData;
	gnssData = {};

	gnssData.clock = clockData;

    gnssData.measurements = measurementsData;
    sGnssMeasureCallback->gnssMeasurementCb(gnssData);
}
#else

int onInit(const sp<IGnssMeasurementCallback>& cb)
{
	return static_cast<std::underlying_type_t<GnssMeasurementStatus>>(GnssMeasurementStatus::ERROR_GENERIC);
}

void onClose(void)
{
}

void sendMeasurements(
    const GnssClock &,
    std::vector<GnssMeasurement>&)
{
}
#endif
} // namespace measurement

namespace navigationMessage {

static Signals signals;

Signals & getSignals() { return signals; }

#ifdef STRAW_ENABLED
int onInit(const sp<IGnssNavigationMessageCallback>& cb)
{
	sGnssNavigationMsgCallback = cb;
	signals.init.emit(sGnssNavigationMsgCallback);

	return 0;
}

void onClose(void)
{
	signals.close.emit();
}

void sendNavigationMessages(GnssNavigationMessage & msg)
{
	ALOGI("SendNavigationMessages");
    sGnssNavigationMsgCallback->gnssNavigationMessageCb(msg);
}
#else
int onInit(const sp<IGnssNavigationMessageCallback>&)
{
	return static_cast<std::underlying_type_t<GnssNavigationMessageStatus>>(GnssNavigationMessageStatus::ERROR_GENERIC);
}

void onClose(void)
{
}

void sendNavigationMessages(GnssNavigationMessage &)
{
}

#endif // STRAW_ENABLED

} // namespace navigationMessage


namespace ril {
	static Signals signals;

	Signals & getSignals() { return signals; }


    void onInit(const sp<IAGnssRilCallback>& cb)
    {
		sAGnssRilCallback = cb;
		signals.init();
	}


	void onSetRefLocation(const IAGnssRil::AGnssRefLocation *agnss_reflocation)
	{
		ALOGI("Set Reference Location received");
		signals.setRefLocation(std::move(agnss_reflocation));
	}

 	void onSetSetId(IAGnssRil::SetIDType type, const char *setid)
	{
		ALOGI("Set Set ID received");
		signals.setSetId(type, setid);
	}

 	void onNiMessage(uint8_t *msg, size_t len)
	{
		ALOGI("Ni message received");
		(void)(len);
		signals.niMessage(msg);
	}

	void onUpdateNetworkState(int connected, int type, int roaming, const char *extra_info)
	{
		ALOGI("Update Network State received");
		(void)(extra_info);
		signals.updateNetworkState(connected,type,roaming);
	}

	void onUpdateNetworkAvailability(int available, const char *apn)
	{
		ALOGI("Update Network Availability received");
		(void)(apn);
		signals.updateNetworkAvailability(available);
	}

	void sendRequestSetId(SetID flags)
	{
		ALOGI("Request Cell Id");
		sAGnssRilCallback->requestSetIdCb(
			static_cast<std::underlying_type_t<SetID>>(flags));
	}

	void sendRequestReferenceLocation(uint32_t flags)
	{
		ALOGI("Request Ref loc");
		sAGnssRilCallback->requestRefLocCb();
	}

} // namespace ril

namespace ni
{
	static Signals signals;

	Signals & getSignals() { return signals; }


	void onInit(const sp<IGnssNiCallback>& cb)
	{
		ALOGI("Initialize AGps interface");
        sGnssNiCallback = cb;
		signals.init();
	}

	void onResponse(int notif_id, IGnssNiCallback::GnssUserResponseType user_response)
	{
		ALOGI("Response to Network Initiated request");
		signals.respond(notif_id, user_response);
	}

	void sendNiNotificationRequest(IGnssNiCallback::GnssNiNotification &notification)
	{
		ALOGI("Send Network Initiated request");
		sGnssNiCallback->niNotifyCb(notification);
	}
} // namespace ni


namespace agps
{
	static Signals signals;

	Signals & getSignals() { return signals; }

    void onInit(const sp<IAGnssCallback>& cb)
	{
		ALOGI("Initialize AGps interface");
        sAGnssCallback = cb;
		signals.init();
	}

	/**
     * Deprecated.
     * If the HAL supports AGpsInterface_v2 this API will not be used, see
     * data_conn_open_with_apn_ip_type for more information.
     */
	int onDataConnOpen(const char* apn)
	{
		ALOGI("Data connection open");
		(void)(apn);
		return 0;
	}

    /**
     * Notifies that the AGPS data connection has been closed.
     */
	int onDataConnClosed(void)
	{
		ALOGI("Data connection closed");
		return 0;
	}
	/**
     * Notifies that a data connection is not available for AGPS.
     */
	int onDataConnFailed(void)
	{
		ALOGI("Data connection failed");
		return 0;
	}

	/**
     * Sets the hostname and port for the AGPS server.
     */
	int onSetServer(AGnssType type, const char* hostname, int port)
	{
		ALOGI("AGps received set server");
		signals.setServer(type,hostname, port);
		return 0;
	}

	/**
     * Notifies that a data connection is available and sets the name of the
     * APN, and its IP type, to be used for SUPL connections.
     */
	int onDataConnOpenWithApnType(const char* apn, ApnIpType apnIpType)
	{
		ALOGI("Data connection available with apn");
		(void)(apn);
		(void)(apnIpType);
		return 0;
	}

    //TODO : check implementation
	void sendAGpsStatusIpV4(AGnssStatusIpV4 status)
	{
		ALOGI("Send agps status IPv4");
	    sAGnssCallback->agnssStatusIpV4Cb(status);
	}

    //TODO : check implementation
	void sendAGpsStatusIpV6(AGnssStatusIpV6 status)
	{
		ALOGI("Send agps status IPv6");
	    sAGnssCallback->agnssStatusIpV6Cb(status);
	}
}

namespace configuration
{
	static Signals signals;

	Signals & getSignals()
	{
		return signals;
	}

	void onConfigurationUpdate(const char* config_data, int32_t length)
	{
		signals.cfgUpdateSig(config_data,length);
	}
}


static std::unordered_map<std::string_view, int> interfacesMap = {
	{GPS_XTRA_INTERFACE,               false},
	{GPS_DEBUG_INTERFACE,              true},
	{SUPL_CERTIFICATE_INTERFACE,       false},
#ifdef AGPS_ENABLED
	{AGPS_INTERFACE,                   true},
	{GPS_NI_INTERFACE,                 true},
	{AGPS_RIL_INTERFACE,               true},
#else
	{AGPS_INTERFACE,                   false},
	{AGPS_RIL_INTERFACE,               false},
	{GPS_NI_INTERFACE,                 false},
#endif
	{GPS_GEOFENCING_INTERFACE,         true},
#ifdef STRAW_ENABLED
	{GPS_MEASUREMENT_INTERFACE,        true},
	{GPS_NAVIGATION_MESSAGE_INTERFACE, true},
#else
	{GPS_MEASUREMENT_INTERFACE,        false},
	{GPS_NAVIGATION_MESSAGE_INTERFACE, false},
#endif
	{GNSS_CONFIGURATION_INTERFACE,     true}
};

namespace gps {

	bool onGetExtension(const char * name)
	{
#ifdef AGPS_ENABLED
		if((std::strcmp(AGPS_INTERFACE,name) == 0) ||
		    (std::strcmp(AGPS_RIL_INTERFACE,name) == 0)||
			(std::strcmp(GPS_NI_INTERFACE,name) == 0))
		{
            // data assistance disabled in configuration
            if(!stm::config::get().agnss.enable)
            {
                return false;
            }
		}
#endif
		auto it  = interfacesMap.find(std::string_view(name));

		if(it != interfacesMap.end())
		{
			ALOGI("Get extension '%s' = %d", name, it->second);
			return it->second;
		}
		else{
			ALOGI("Extension '%s' not supported: NULL", name);
			return false;
		}
	}
} // namespace gps


} // namespace LocServiceProxy
} // namespace stm
