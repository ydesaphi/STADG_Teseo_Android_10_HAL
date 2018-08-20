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
 * @file manager.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/geofencing/manager.h>

#define LOG_TAG "teseo_hal_GeofencingManager"
#include <cutils/log.h>

using namespace stm::geofencing::model;

namespace stm {
namespace geofencing {

void GeofencingManager::initialize()
{
    ALOGI("GeofencingManager init");
}

//void GeofencingManager::add(GeofenceDefinition && def)
void GeofencingManager::add(GeofenceDefinition def)
{
    GeofenceId id = def.id;

    auto it = this->geofences.find(id);

    if(it != this->geofences.end()) {
        ALOGE("Unable to add geofence #%d, id already exists", id);
        this->answerGeofenceAddRequest(id, OperationStatus::Error_IdExists);
        return;
    }

    if(!transitionFlagsIsValid(def.monitor_transitions)) {
        ALOGE("Unable to add geofence #%d, transition flag (0x%x) is not valid", id, static_cast<int32_t>(def.monitor_transitions));
        this->answerGeofenceAddRequest(id, OperationStatus::Error_InvalidTransition);
        return;
    }
    
    try {
        auto geofence_ptr = std::make_unique<Geofence>(std::move(def),this);
        this->geofences.insert(std::make_pair(id, std::move(geofence_ptr)));

        ALOGI("Geofence #%d added, now tracking %d geofences", id, (int)std::count_if(geofences.begin(), geofences.end(), [](auto & p) {
            return p.second->trackingStatus() == Geofence::TrackingStatus::Tracking;
        }));

        this->answerGeofenceAddRequest(id, OperationStatus::Success);
    } catch(const std::exception & e) {
        ALOGE("Exception while adding geofence %d: %s", id, e.what());
        this->answerGeofenceAddRequest(id, OperationStatus::Error_Generic);
    }
}

void GeofencingManager::remove(GeofenceId id)
{
    auto iterator = this->geofences.find(id);
    if(iterator != this->geofences.end()) {
        // Maybe we don't need the removing tracking status
        iterator->second->setTrackingStatus(Geofence::TrackingStatus::Removing);
        iterator->second.reset();
        this->geofences.erase(iterator);
        ALOGI("Geofence #%d removed.", id);
        this->answerGeofenceRemoveRequest(id, OperationStatus::Success);
    } else {
        ALOGE("Geofence #%d not found, can't remove it.",id);
        this->answerGeofenceRemoveRequest(id, OperationStatus::Error_IdUnknown);
    }
}

void GeofencingManager::pause(GeofenceId id)
{
    auto iterator = this->geofences.find(id);
    if(iterator != this->geofences.end()) {
        iterator->second->setTrackingStatus(Geofence::TrackingStatus::Paused);
        ALOGI("Geofence #%d paused.",id);
        this->answerGeofencePauseRequest(id, OperationStatus::Success);
    } else {
        ALOGE("Geofence #%d not found, can't pause it.",id);
        this->answerGeofencePauseRequest(id, OperationStatus::Error_IdUnknown);
    }
}

void GeofencingManager::resume(GeofenceId id, TransitionFlags monitored_transitions)
{
    auto iterator = this->geofences.find(id);
    if(iterator != this->geofences.end()) {
        iterator->second->setMonitoredTransition(monitored_transitions);
        iterator->second->setTrackingStatus(Geofence::TrackingStatus::Tracking);
        ALOGI("Geofence #%d resumed.",id);
        this->answerGeofenceResumeRequest(id, OperationStatus::Success);
    } else {
        ALOGE("Geofence #%d not found, can't resume it.",id);
        this->answerGeofenceResumeRequest(id, OperationStatus::Error_IdUnknown);
    }
}

void GeofencingManager::onLocationUpdate(const Location & loc)
{
    ALOGI("onLocationUpdate");

    m_lastLocation = loc;

    for(auto & pair : geofences)
    {
        auto & geofence_ptr = pair.second;
        geofence_ptr->updateStatusFromLocation(loc);
    }
}

void GeofencingManager::onDeviceStatusUpdate(GpsStatusValue deviceStatus)
{
    /*
     The current HAL doesn't give ENGINE_ON and ENGINE_OFF status which should be used for that purpose
     When they will be available this function should not depend on the SESSION_{BEGIN,END} statuses.
     */
    switch(deviceStatus)
    {
        //case GPS_STATUS_ENGINE_ON:
        case GPS_STATUS_SESSION_BEGIN:
            sendGeofenceStatus(SystemStatus::Available, m_lastLocation);
            break;

        //case GPS_STATUS_ENGINE_OFF:
        case GPS_STATUS_SESSION_END:
        case GPS_STATUS_NONE:
        default:
            sendGeofenceStatus(SystemStatus::Unavailable, m_lastLocation);
            break;
    }
}

GeofencingManager::~GeofencingManager(){

    //delete m_lastLocation_ptr;
}

} // namespace device
} // namespace stm