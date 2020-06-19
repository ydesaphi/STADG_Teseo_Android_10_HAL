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
 * @file Location.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/model/Location.h>
#include <sstream>

namespace stm {

using namespace model;

Location::Location()
{
	this->invalidateAll();
	this->_fixQuality = FixQuality::Invalid;
	this->_fixMode = FixMode::NoFix;
}

FixQuality Location::quality() const
{
	return this->_fixQuality;
}

FixMode Location::fixMode() const
{
	return this->_fixMode;
}

double Location::latitude() const
{
	return _latitude;
}

double Location::longitude() const
{
	return _longitude;
}

std::pair<double, double> Location::location() const
{
	return std::make_pair(_latitude, _longitude);
}

double Location::altitude() const
{
	return _altitude;
}

float Location::speed() const
{
	return _speed;
}

float Location::bearing() const
{
	return _bearing;
}

float Location::accuracy() const
{
	return _accuracy;
}

GnssUtcTime Location::timestamp() const
{
	return _timestamp;
}

FixQuality Location::quality(FixQuality value)
{
	this->_fixQuality = value;
	return this->_fixQuality;
}

FixMode Location::fixMode(FixMode value)
{
	this->_fixMode = value;
	return this->_fixMode;
}

std::pair<double, double> Location::location(double latitude, double longitude)
{
	hasLatLong = true;
	_latitude = latitude;
	_longitude = longitude;
	return std::make_pair(_latitude, _longitude);
}

double Location::altitude(double value)
{
	hasAltitude = true;
	_altitude = value;
	return _altitude;
}

float Location::speed(float value)
{
	hasSpeed = true;
	_speed = value;
	return _speed;
}

float Location::bearing(float value)
{
	hasBearing = true;
	_bearing = value;
	return _bearing;
}

float Location::accuracy(float value)
{
	hasAccuracy = true;
	_accuracy = value;
	return _accuracy;
}

GnssUtcTime Location::timestamp(GnssUtcTime value)
{
	_timestamp = value;
	return _timestamp;
}

void Location::copyToGnssLocation(GnssLocation & loc) const
{
    // Bit operation AND with 1f below is needed to clear vertical accuracy,
    // speed accuracy and bearing accuracy flags as some vendors are found
    // to be setting these bits in pre-Android-O devices
    loc.gnssLocationFlags               = static_cast<uint16_t>((
        (hasLatLong  ? static_cast<std::underlying_type_t<GnssLocationFlags>>(GnssLocationFlags::HAS_LAT_LONG) : 0x0000) |
        (hasAltitude ? static_cast<std::underlying_type_t<GnssLocationFlags>>(GnssLocationFlags::HAS_ALTITUDE) : 0x0000) |
        (hasSpeed    ? static_cast<std::underlying_type_t<GnssLocationFlags>>(GnssLocationFlags::HAS_SPEED)    : 0x0000) |
        (hasBearing  ? static_cast<std::underlying_type_t<GnssLocationFlags>>(GnssLocationFlags::HAS_BEARING)  : 0x0000) |
        (hasAccuracy ? static_cast<std::underlying_type_t<GnssLocationFlags>>(GnssLocationFlags::HAS_HORIZONTAL_ACCURACY) : 0x0000) )
        & 0x1f);
    loc.latitudeDegrees                 = this->_latitude;
    loc.longitudeDegrees                = this->_longitude;
    loc.altitudeMeters                  = this->_altitude;
    loc.speedMetersPerSec               = this->_speed;
    loc.bearingDegrees                  = this->_bearing;
    loc.horizontalAccuracyMeters        = this->_accuracy;
    // Older chipsets do not provide the following 3 fields, hence the flags
    // HAS_VERTICAL_ACCURACY, HAS_SPEED_ACCURACY and HAS_BEARING_ACCURACY are
    // not set and the field are set to zeros.
    loc.verticalAccuracyMeters          = 0;
    loc.speedAccuracyMetersPerSecond    = 0;
    loc.bearingAccuracyDegrees          = 0;
    loc.timestamp                       = this->_timestamp;
}

std::string Location::toString() const
{
	std::ostringstream buffer;

	buffer << "time = " << _timestamp << ", pos = [" << _latitude << ", " << _longitude
	       << "], alt = " << _altitude << ", speed = " << _speed
	       << ", bearing = " << _bearing << ", accuracy = " << _accuracy;

	return buffer.str();
}

bool Location::locationValidity() const
{
	return hasLatLong;
}

bool Location::altitudeValidity() const
{
	return hasAltitude;
}

bool Location::speedValidity() const
{
	return hasSpeed;
}

bool Location::bearingValidity() const
{
	return hasBearing;
}

bool Location::accuracyValidity() const
{
	return hasAccuracy;
}

void Location::invalidateLocation()
{
	hasLatLong = false;
}

void Location::invalidateAltitude()
{
	hasAltitude = false;
}

void Location::invalidateSpeed()
{
	hasSpeed = false;
}

void Location::invalidateBearing()
{
	hasBearing = false;
}

void Location::invalidateAccuracy()
{
	hasAccuracy = false;
}

void Location::invalidateAll()
{
	hasLatLong  = false;
	hasAltitude = false;
	hasSpeed    = false;
	hasBearing  = false;
	hasAccuracy = false;
}

} // namespace stm
