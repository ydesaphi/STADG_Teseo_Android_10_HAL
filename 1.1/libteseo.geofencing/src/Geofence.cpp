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
 * @file Geofence.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/geofencing/model.h>
#include <teseo/geofencing/Geofence.h>
#include <teseo/geofencing/manager.h>

namespace stm {
namespace geofencing {

using namespace stm::geofencing::model;


Geofence::Geofence() {}

Geofence::Geofence(const model::GeofenceDefinition def, GeofencingManager * mng) {
    id_ = def.id;
    radius_ = def.radius;
    origin_.latitude = def.origin.latitude;
    origin_.longitude = def.origin.longitude;
    last_transition_ = def.last_transition;
    monitor_transitions_ = def.monitor_transitions;
    notifications_responsiveness_ = def.notifications_responsiveness;
    unknown_time_ = def.unknown_time;
    status_ = TrackingStatus::Tracking;
    state_ = State::Unknown;
    last_transition_time = time_point();
    manager = mng;
}

bool Geofence::isMonitored(Transition t) const
{
    return static_cast<int32_t>(t) & monitor_transitions_;
}

void Geofence::setMonitoredTransition(TransitionFlags flags)
{
    this->monitor_transitions_ = flags;
}

void Geofence::updateStatusFromLocation(const Location & loc)
{
    const Point p(loc);
    const double distance = origin_.distanceFrom(p);

    // First estimation of new state without location accurracy
    State s = (distance <= radius_) ? State::Inside : State::Outside;

    // Second estimation of new state using location accuracy
    if(s == State::Inside)
        s = distance + loc.accuracy() <= radius_ ? State::Inside : State::Unknown;
    else
        s = distance + loc.accuracy() <= radius_ ? State::Unknown : State::Outside;

    auto t = last_transition_;

    switch(state_)
    {
    case State::Inside:
        switch(s)
        {
        case State::Outside:
            t = Transition::Exited;
            break;
        
        case State::Unknown:
            t = Transition::Uncertain;
            break;
        default:
            break;
        }
        break;

    case State::Outside:
        switch(s)
        {
        case State::Inside:
            t = Transition::Entered;
            break;

        case State::Unknown:
            t = Transition::Uncertain;
            break;
        default:
            break;
        }
        break;

    case State::Unknown:
        switch(s)
        {
        case State::Inside:
            t = Transition::Entered;
            break;

        case State::Outside:
            t = Transition::Exited;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }


    if(t != last_transition_)
    {
        last_transition_ = t;
        last_transition_time = std::chrono::system_clock().now();

        if(isMonitored(last_transition_) && status_ == TrackingStatus::Tracking)
            manager->sendGeofenceTransition(id_, loc, last_transition_, loc.timestamp());
    }
}

} // namespace geofencing
} // namespace stm