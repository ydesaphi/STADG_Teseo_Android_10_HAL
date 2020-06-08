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
 * @file model.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/geofencing/model.h>

#include <cmath>
#include <boost/math/constants/constants.hpp>


namespace stm {
namespace geofencing {
namespace model {

double degree_to_rad(double d)
{
    return d * boost::math::double_constants::degree;
}

bool transitionFlagsIsValid(TransitionFlags flags)
{
    constexpr TransitionFlags ALL_FLAGS_INVERTED = ~(
        static_cast<int32_t>(Transition::Entered) &
        static_cast<int32_t>(Transition::Exited)  &
        static_cast<int32_t>(Transition::Uncertain));

    // force all valid values in flags to zero
    // if others bit are enabled transition flags is not valie
    return !(flags & ALL_FLAGS_INVERTED);
}

constexpr double WGS84_A = 6378137.0;
constexpr double WGS84_E = 0.0818191908426;
constexpr double WGS84_E2 = WGS84_E * WGS84_E;

Point::Point()
{ }

Point::Point(const Location & loc) :
    latitude(loc.latitude()),
    longitude(loc.longitude())
{ }

Point::Point(const ICoordinate & lat, const ICoordinate & lon) :
    latitude(lat.asDecimalDegree()),
    longitude(lon.asDecimalDegree())
{ }

std::pair<double, double> Point::to_rad() const
{
    return std::make_pair(
        degree_to_rad(latitude.value()),
        degree_to_rad(longitude.value())
    );
}

double Point::distanceFrom(const Point & p)
{
    const auto this_rad = to_rad();
    const auto p_rad = p.to_rad();

    const double sin_lat = sin(this_rad.first);

    /* Earth local radius @ given latitude */
    const double N = WGS84_A / sqrt(1.0 - (WGS84_E2 * sin_lat * sin_lat));

    return sqrt(
        ((N * (p_rad.first - this_rad.first)) * (N * (p_rad.first - this_rad.first))) +
        ((N * (p_rad.second - this_rad.second) * sin_lat) * (N * (p_rad.second - this_rad.second) * sin_lat))
    );
}

} // namespace model
} // namespace geofencing
} // namespace stm