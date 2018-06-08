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
#ifndef TESEO_HAL_GEOFENCING_MANAGER_H
#define TESEO_HAL_GEOFENCING_MANAGER_H

#include <unordered_map>
#include <memory>
#include <forward_list>

#include <teseo/geofencing/model.h>
#include <teseo/geofencing/Geofence.h>
#include <teseo/utils/Signal.h>

namespace stm {
namespace geofencing {

class GeofencingManager {
private:

    std::unordered_map<model::GeofenceId, std::shared_ptr<Geofence>> geofences;

public:

    /**
     * Initialize the geofencing manager
     */
    void initialize();

    /**
     * @brief Add a new geofence to track
     * @param def Geofence definition informations
     */
    void add(model::GeofenceDefinition && def);

    /**
     * @brief Remove a geofence from tracking
     * @param id Identifier of the geofence to remove
     */
    void remove(model::GeofenceId id);

    /**
     * @brief Pause geofence tracking
     * @param id Identifier of the geofence to pause
     */
    void pause(model::GeofenceId id);

    /**
     * @brief Resume geofence tracking
     * @param id Identifier of the geofence to resume
     * @param watched_transitions Transitions to watch
     */
    void resume(model::GeofenceId id, model::TransitionFlags watched_transitions);

    void onLocationUpdate(const Location &);

    Signal<void, model::SystemStatus> sendGeofenceStatus;

    Signal<void, model::GeofenceId, const Location &, model::Transition, GpsUtcTime> sendGeofenceTransition;

    Signal<void, model::GeofenceId, model::OperationStatus> answerGeofenceAddRequest;

    Signal<void, model::GeofenceId, model::OperationStatus> answerGeofenceRemoveRequest;

    Signal<void, model::GeofenceId, model::OperationStatus> answerGeofencePauseRequest;

    Signal<void, model::GeofenceId, model::OperationStatus> answerGeofenceResumeRequest;
};

} // namespace geofencing
} // namespace stm

#endif