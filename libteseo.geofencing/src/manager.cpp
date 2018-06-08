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
    // Nothing to do for now
}

void GeofencingManager::add(GeofenceDefinition && def)
{
    GeofenceId id = def.id;

    auto it = this->geofences.find(id);

    if(it != this->geofences.end()) {
        this->answerGeofenceAddRequest(id, OperationStatus::Error_IdExists);
        return;
    }

    if(!transitionFlagsIsValid(def.monitor_transitions)) {
        this->answerGeofenceAddRequest(id, OperationStatus::Error_InvalidTransition);
        return;
    }
    
    try {
        auto geofence_ptr = std::make_shared<Geofence>(std::move(def));
        this->geofences.insert(std::make_pair(id, geofence_ptr));
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
        this->answerGeofenceRemoveRequest(id, OperationStatus::Success);
    } else {
        this->answerGeofenceRemoveRequest(id, OperationStatus::Error_IdUnknown);
    }
}

void GeofencingManager::pause(GeofenceId id)
{
    auto iterator = this->geofences.find(id);
    if(iterator != this->geofences.end()) {
        iterator->second->setTrackingStatus(Geofence::TrackingStatus::Paused);
        this->answerGeofencePauseRequest(id, OperationStatus::Success);
    } else {
        this->answerGeofencePauseRequest(id, OperationStatus::Error_IdUnknown);
    }
}

void GeofencingManager::onLocationUpdate(const Location & loc)
{
}

} // namespace device
} // namespace stm