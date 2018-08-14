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
#include <string_view>
#include <teseo/config/config.h>

#include <teseo/HalManager.h>
#include <teseo/utils/Thread.h>

namespace stm {
namespace LocServiceProxy {

static struct {
	GpsCallbacks gps;
	GpsGeofenceCallbacks geofence;
#ifdef STRAW_ENABLED
	GpsMeasurementCallbacks measurement;
	GpsNavigationMessageCallbacks navigationMessage;
#endif
	AGpsRilCallbacks ril;
	GpsNiCallbacks ni;
	AGpsCallbacks agps;
} callbacks;

static struct gps_device_t * device = nullptr;


void RegisterCallbacks(const GpsCallbacks * cb)
{
	Thread::setCreateThreadCb(callbacks.gps.create_thread_cb);
	memcpy(&(callbacks.gps), cb, sizeof(GpsCallbacks));
}

void RegisterGeofenceCallbacks(const GpsGeofenceCallbacks * cb)
{
	memcpy(&(callbacks.geofence), cb, sizeof(GpsGeofenceCallbacks));
}

#ifdef STRAW_ENABLED
void RegisterNavigationMessageCallbacks(const GpsNavigationMessageCallbacks * cb)
{
	memcpy(&(callbacks.navigationMessage), cb, sizeof(GpsNavigationMessageCallbacks));
}

void RegisterMeasurementsCallbacks(const GpsMeasurementCallbacks * cb)
{
	memcpy(&(callbacks.measurement), cb, sizeof(GpsMeasurementCallbacks));
}
#endif
void RegisterRilCallbacks(const AGpsRilCallbacks *cb)
{
	memcpy(&(callbacks.ril), cb, sizeof(AGpsRilCallbacks));
}

void RegisterNiCallbacks(const GpsNiCallbacks *cb)
{
	memcpy(&(callbacks.ni), cb, sizeof(GpsNiCallbacks));
}

void RegisterAGpsCallbacks(const AGpsCallbacks *cb)
{
	memcpy(&(callbacks.agps), cb, sizeof(AGpsCallbacks));
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

namespace geofencing {

using namespace stm::geofencing::model;

static Signals signals;

Signals & getSignals() { return signals; }

void onInit(GpsGeofenceCallbacks * callbacks)
{
	ALOGI("Initialize geofence system request");
	RegisterGeofenceCallbacks(callbacks);
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

void sendGeofenceTransition(GeofenceId geofence_id,  const Location & loc, Transition transition, GpsUtcTime timestamp)
{
	// Convert location to Android location format
	GpsLocation location;
	loc.copyToGpsLocation(location);

	ALOGI("Send geofence transition: id=%d, loc=%s", geofence_id, loc.toString().c_str());
	callbacks.geofence.geofence_transition_callback(geofence_id, &location, static_cast<int32_t>(transition), timestamp);
}

void sendGeofenceStatus(SystemStatus status, const Location & last_location)
{
	GpsLocation location;
	last_location.copyToGpsLocation(location);

	ALOGI("Send geofence system status: %d", static_cast<int32_t>(status));
	callbacks.geofence.geofence_status_callback(static_cast<int32_t>(status), &location);
}

void answerGeofenceAddRequest(GeofenceId geofence_id, OperationStatus status)
{
	ALOGI("Answer geofence add request; id=%d, result=%d", geofence_id, static_cast<int32_t>(status));
	callbacks.geofence.geofence_add_callback(geofence_id, static_cast<int32_t>(status));
}

void answerGeofenceRemoveRequest(GeofenceId geofence_id, OperationStatus status)
{
	ALOGI("Answer geofence remove request; id=%d, result=%d", geofence_id, static_cast<int32_t>(status));
	callbacks.geofence.geofence_remove_callback(geofence_id, static_cast<int32_t>(status));
}

void answerGeofencePauseRequest(GeofenceId geofence_id, OperationStatus status)
{
	ALOGI("Answer geofence pause request; id=%d, result=%d", geofence_id, static_cast<int32_t>(status));
	callbacks.geofence.geofence_pause_callback(geofence_id, static_cast<int32_t>(status));
}

void answerGeofenceResumeRequest(GeofenceId geofence_id, OperationStatus status)
{
	ALOGI("Answer geofence resume request; id=%d, result=%d", geofence_id, static_cast<int32_t>(status));
	callbacks.geofence.geofence_resume_callback(geofence_id, static_cast<int32_t>(status));
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

#ifdef STRAW_ENABLED
namespace measurement {
static Signals signals;

Signals & getSignals()
{
	return signals;
}

int  onInit(GpsMeasurementCallbacks * cb)
{
   	RegisterMeasurementsCallbacks(cb);
	signals.init.emit(cb);
	return 0;
}
void onClose(void)
{
   	signals.close.emit();
}

void sendMeasurements(const GnssClock & clockData, std::vector <GnssMeasurement> & measurementdata)
{
	ALOGI("SendMeasurements");

	GnssData Measurementmsg = {.size = sizeof(GnssData), .measurement_count = 0};
	Measurementmsg.clock = clockData;
	Measurementmsg.measurement_count = measurementdata.size();
	int i = 0;
	for(std::vector<GnssMeasurement>::iterator it = measurementdata.begin(); it != measurementdata.end(); it++,i++ )    {
		if(i == GNSS_MAX_MEASUREMENT) {
			break;
		}
		Measurementmsg.measurements[i] = *it ;
	}
	callbacks.measurement.gnss_measurement_callback( &Measurementmsg);
}

}

namespace navigationMessage {

static Signals signals;

Signals & getSignals() { return signals; }

int onInit(GpsNavigationMessageCallbacks * callbacks)
{
	RegisterNavigationMessageCallbacks(callbacks);
	signals.init.emit(callbacks);
	return 0;
}

void onClose(void)
{
	signals.close.emit();
}

void sendNavigationMessages(GnssNavigationMessage & msg)
{
	ALOGI("SendNavigationMessages");
	callbacks.navigationMessage.gnss_navigation_message_callback(static_cast<GnssNavigationMessage *> (&msg));
}
} //end navigation message

#endif
namespace ril {
	static Signals signals;

	Signals & getSignals() { return signals; }


	void onInit(AGpsRilCallbacks *callbacks)
	{
		ALOGI("Initialize ril interface");
		RegisterRilCallbacks(callbacks);
		signals.init();
	}

	void onSetRefLocation(const AGpsRefLocation *agps_reflocation, size_t sz_struct)
	{
		ALOGI("Set Reference Location received");
		(void)(sz_struct);
		signals.setRefLocation(std::move(agps_reflocation));
	}

 	void onSetSetId( AGpsSetIDType type, const char *setid)
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

	void sendRequestSetId(uint32_t flags)
	{
		ALOGI("Request Cell Id");
		callbacks.ril.request_setid(flags);
	}

	void sendRequestReferenceLocation(uint32_t flags)
	{
		ALOGI("Request Ref loc");
		callbacks.ril.request_refloc(flags);
	}

} // namespace ril

namespace ni
{
	static Signals signals;

	Signals & getSignals() { return signals; }

	void onInit(GpsNiCallbacks *callbacks)
	{
		ALOGI("Initialize Ni interface");
		RegisterNiCallbacks(callbacks);
		signals.init();
	}

	void onResponse(int notif_id, GpsUserResponseType user_response)
	{
		ALOGI("Response to Network Initiated request");
		signals.respond(notif_id, user_response);
	}

	void sendNiNotificationRequest(GpsNiNotification *notification)
	{
		ALOGI("Send Network Initiated request");
		callbacks.ni.notify_cb(notification);
	}
} // namespace ni


namespace agps
{
	static Signals signals;

	Signals & getSignals() { return signals; }

	void onInit(AGpsCallbacks *callbacks)
	{
		ALOGI("Initialize AGps interface");
		RegisterAGpsCallbacks(callbacks);
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
	int onSetServer(AGpsType type, const char* hostname, int port)
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


	void sendAGpsStatus(AGpsStatus* status)
	{
		ALOGI("Send agps status");
		callbacks.agps.status_cb(status);
	}
}

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
	.geofencing = {
		.size                 = sizeof(GpsGeofencingInterface),
		.init                 = &LocServiceProxy::geofencing::onInit,
		.add_geofence_area    = &LocServiceProxy::geofencing::onAddGeofenceArea,
		.pause_geofence       = &LocServiceProxy::geofencing::onPauseGeofence,
		.resume_geofence      = &LocServiceProxy::geofencing::onResumeGeofence,
		.remove_geofence_area = &LocServiceProxy::geofencing::onRemoveGeofenceArea
	},
	.debug = {
		.size               = sizeof(GpsDebugInterface),
		.get_internal_state = &LocServiceProxy::debug::getInternalState
	},
#ifdef STRAW_ENABLED
	.measurement = {
		.size               = sizeof(GpsMeasurementInterface),
		.init               = &LocServiceProxy::measurement::onInit,
		.close              = &LocServiceProxy::measurement::onClose
	},
	.navigationMessage = {
		.size                 = sizeof(GpsNavigationMessageInterface),
		.init                 = &LocServiceProxy::navigationMessage::onInit,
		.close              = &LocServiceProxy::navigationMessage::onClose
	},
#endif
	.ril = {
		.size				= sizeof(AGpsRilInterface),
		.init				= &LocServiceProxy::ril::onInit,
		.set_ref_location	= &LocServiceProxy::ril::onSetRefLocation,
		.set_set_id			= &LocServiceProxy::ril::onSetSetId,
		.ni_message         = &LocServiceProxy::ril::onNiMessage,
		.update_network_state= &LocServiceProxy::ril::onUpdateNetworkState,
		.update_network_availability=&LocServiceProxy::ril::onUpdateNetworkAvailability
	},
	.ni = {
		.size				= sizeof(GpsNiInterface),
		.init				= &LocServiceProxy::ni::onInit,
		.respond			= &LocServiceProxy::ni::onResponse
	},
	.agps = {
		.size				= sizeof(AGpsInterface),
		.init				= &LocServiceProxy::agps::onInit,
		.data_conn_open		= &LocServiceProxy::agps::onDataConnOpen,
		.data_conn_closed	= &LocServiceProxy::agps::onDataConnClosed,
		.data_conn_failed	= &LocServiceProxy::agps::onDataConnFailed,
		.set_server			= &LocServiceProxy::agps::onSetServer,
		.data_conn_open_with_apn_ip_type =&LocServiceProxy::agps::onDataConnOpenWithApnType	
	}

};

static std::unordered_map<std::string_view, void *> interfacesMap = {
	{GPS_XTRA_INTERFACE,               NULL}                     ,
	{GPS_DEBUG_INTERFACE,              &(interfaces.debug)}      ,
	{SUPL_CERTIFICATE_INTERFACE,       NULL}                     ,
#ifdef AGPS_ENABLED
	{AGPS_INTERFACE,                   &(interfaces.agps)}       ,
	{GPS_NI_INTERFACE,                 &(interfaces.ni)}         ,
	{AGPS_RIL_INTERFACE,               &(interfaces.ril)}        ,
#else
	{AGPS_INTERFACE,                   NULL}                     ,
	{AGPS_RIL_INTERFACE,               NULL} 				     ,
	{GPS_NI_INTERFACE,                 NULL}                     ,
#endif
	{GPS_GEOFENCING_INTERFACE,         &(interfaces.geofencing)} ,
#ifdef STRAW_ENABLED
	{GPS_MEASUREMENT_INTERFACE,        &(interfaces.measurement)}   	,
	{GPS_NAVIGATION_MESSAGE_INTERFACE, &(interfaces.navigationMessage)}	,
#else
	{GPS_MEASUREMENT_INTERFACE,        NULL}				,
	{GPS_NAVIGATION_MESSAGE_INTERFACE, NULL}				,
#endif
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
		

#ifdef AGPS_ENABLED
		if((std::strcmp(AGPS_INTERFACE,name)==0)||
			(std::strcmp(AGPS_RIL_INTERFACE,name)==0)||
			(std::strcmp(GPS_NI_INTERFACE,name)==0))
			{
				// data assistance disabled in configuration
				if(!config::get().agnss.enable)
				{
					return NULL;
				}
			}
#endif
		auto it  = interfacesMap.find(std::string_view(name));

		if(it != interfacesMap.end())
		{
			ALOGI("Get extension '%s': %p", name, it->second);
			return it->second;
		}
		else{
			ALOGI("Extension '%s' not supported: NULL", name);
			return NULL;
		}
	}
}


} // namespace LocServiceProxy
} // namespace stm
