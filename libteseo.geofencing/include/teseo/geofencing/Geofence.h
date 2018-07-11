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
#ifndef TESEO_HAL_GEOFENCING_GEOFENCE_H
#define TESEO_HAL_GEOFENCING_GEOFENCE_H

#include <chrono>

#include <teseo/model/Coordinate.h>
#include <teseo/geofencing/model.h>
#include <teseo/model/Location.h>

namespace stm {
namespace geofencing {

class GeofencingManager;

class Geofence {
public:
    enum class TrackingStatus {
        Tracking,
        Paused,
        Removing
    };

private:
    using time_point = std::chrono::time_point<std::chrono::system_clock>;
    using milliseconds = std::chrono::milliseconds;

    model::GeofenceId id_;

    double radius_;
    
    model::Point origin_;

    model::Transition last_transition_;
    model::TransitionFlags monitor_transitions_;

    milliseconds notifications_responsiveness_;
    milliseconds unknown_time_;

    TrackingStatus status_;

    enum class State {
        Inside,
        Outside,
        Unknown
    };

    State state_;

    time_point last_transition_time;

    GeofencingManager * manager;

public:
    Geofence();
    /**
     * Create a geofence
     * @param def Definition of geofence
     * @param manager Manager of the geofence
     */
    Geofence(const model::GeofenceDefinition def, GeofencingManager * manager);

    model::GeofenceId id() const { return id_; }

    double radius() const { return radius_; }

    model::Point origin() const { return origin_; }

    model::Transition lastTransition() const { return last_transition_; }

    model::TransitionFlags monitoredTransitions() const { return monitor_transitions_; }

    std::chrono::milliseconds notificationResponsivennes() const { return notifications_responsiveness_; }

    std::chrono::milliseconds unknownTime() const { return unknown_time_; }

    TrackingStatus trackingStatus() const { return status_; }

    void setTrackingStatus(TrackingStatus status) { status_ = status; }

    void setMonitoredTransition(model::TransitionFlags flags);

    void updateStatusFromLocation(const Location & loc);

    bool isMonitored(model::Transition t) const;
};

} // namespace geofencing
} // namespace stm

#endif