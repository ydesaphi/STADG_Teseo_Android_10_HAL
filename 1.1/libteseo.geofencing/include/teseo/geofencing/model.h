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

#ifndef TESEO_HAL_GEOFENCING_MODEL_H
#define TESEO_HAL_GEOFENCING_MODEL_H

#include <chrono>

#include <teseo/model/Location.h>
#include <teseo/model/Coordinate.h>

#include <teseo/utils/Gnss_1_1.h>

namespace stm {
namespace geofencing {
namespace model {

using GeofenceId = int32_t;

/**
 * Geofence transitions
 */
enum class Transition {
    Entered   = static_cast<std::underlying_type_t<GeofenceTransition>>(GeofenceTransition::ENTERED),
    Exited    = static_cast<std::underlying_type_t<GeofenceTransition>>(GeofenceTransition::EXITED),
    Uncertain = static_cast<std::underlying_type_t<GeofenceTransition>>(GeofenceTransition::UNCERTAIN),
};

/**
 * Geofence transition flag
 */
using TransitionFlags = int;

/**
 * Geofence system status/availability
 */
enum class SystemStatus {
    Unavailable = static_cast<std::underlying_type_t<GeofenceAvailability>>(GeofenceAvailability::UNAVAILABLE),
    Available   = static_cast<std::underlying_type_t<GeofenceAvailability>>(GeofenceAvailability::AVAILABLE),
};

/**
 * Geofence operation status/result
 */
enum class OperationStatus {
    Success                 = static_cast<std::underlying_type_t<GeofenceStatus>>(GeofenceStatus::OPERATION_SUCCESS),
    Error_TooManyGeofences  = static_cast<std::underlying_type_t<GeofenceStatus>>(GeofenceStatus::ERROR_TOO_MANY_GEOFENCES),
    Error_IdExists          = static_cast<std::underlying_type_t<GeofenceStatus>>(GeofenceStatus::ERROR_ID_EXISTS),
    Error_IdUnknown         = static_cast<std::underlying_type_t<GeofenceStatus>>(GeofenceStatus::ERROR_ID_UNKNOWN),
    Error_InvalidTransition = static_cast<std::underlying_type_t<GeofenceStatus>>(GeofenceStatus::ERROR_INVALID_TRANSITION),
    Error_Generic           = static_cast<std::underlying_type_t<GeofenceStatus>>(GeofenceStatus::ERROR_GENERIC),
};

/**
 * Simple 2D point structure
 */
struct Point {
    /**
     * @brief Default constructor (lat=0, long=0)
     */
    Point();

    /**
     * @brief Create a point from a location
     * @param loc Location to use
     */
    Point(const Location & loc);

    /**
     * @brief Create a point from coordinates
     * @param lat Latitude
     * @param lon Longitude
     */
    Point(const ICoordinate & lat, const ICoordinate & lon);

    DecimalDegreeCoordinate latitude;
    DecimalDegreeCoordinate longitude;

    /**
     * Calculate distance from 2 points
     */
    double distanceFrom(const Point & p);

private:
    /**
     * Get coordinates as radians instead of decimale degrees
     * @return Latitude and longitude as pair in this order
     */
    std::pair<double, double> to_rad() const;
};

/**
 * Geofence definition
 */
struct GeofenceDefinition {
    GeofenceId id;
    Point origin;
    double radius;
    Transition last_transition;
    TransitionFlags monitor_transitions;
    std::chrono::milliseconds notifications_responsiveness;
    std::chrono::milliseconds unknown_time;
};

/**
 * Check if transition flag is valid
 * @param flags Transition flag to check
 * @return true if valid, false otherwise
 */
bool transitionFlagsIsValid(TransitionFlags flags);

} // namespace model
} // namespace geofencing
} // namespace stm

#endif // TESEO_HAL_GEOFENCING_MODEL_H
