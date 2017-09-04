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
 * @file Location.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_MODEL_LOCATION
#define TESEO_HAL_MODEL_LOCATION

#include <hardware/gps.h>
#include <teseo/utils/ByteVector.h>

#include "FixAndOperatingModes.h"
#include "FixQuality.h"

namespace stm {

/**
 * @brief      Location class
 */
class Location {
private:

	/** Location quality */
	FixQuality _fixQuality;

	/** Fix mode */
	model::FixMode _fixMode;

	/** Represents latitude in degrees. */
    double          _latitude;
    
	/** Represents longitude in degrees. */
    double          _longitude;

	bool hasLatLong;
    
	/**
     * Represents altitude in meters above the WGS 84 reference ellipsoid.
     */
    double          _altitude;

	bool hasAltitude;
    
	/** Represents speed in meters per second. */
    float           _speed;

	bool hasSpeed;
    
	/** Represents heading in degrees. */
    float           _bearing;

	bool hasBearing;
    
	/** Represents expected accuracy in meters. */
    float           _accuracy;

	bool hasAccuracy;

    /** Timestamp for the location fix. */
    GpsUtcTime      _timestamp;

public:
	Location();

	/**
	 * @brief      Get location validity
	 */
	bool locationValidity() const;

	/**
	 * @brief      Get altitude validity
	 */
	bool altitudeValidity() const;

	/**
	 * @brief      Get speed validity
	 */
	bool speedValidity() const;

	/**
	 * @brief      Get bearing validity
	 */
	bool bearingValidity() const;

	/**
	 * @brief      Get accuracy validity
	 */
	bool accuracyValidity() const;

	/**
	 * @brief      Invalidate all location data
	 */
	void invalidateAll();

	/**
	 * @brief      Invalidate location
	 */
	void invalidateLocation();

	/**
	 * @brief      Invalidate altitude
	 */
	void invalidateAltitude();

	/**
	 * @brief      Invalidate speed
	 */
	void invalidateSpeed();

	/**
	 * @brief      Invalidate bearing
	 */
	void invalidateBearing();

	/**
	 * @brief      Invalidate accuracy
	 */
	void invalidateAccuracy();

	/**
	 * @brief      Get the fix quality
	 */
	FixQuality quality() const;

	/**
	 * @brief      Get the fix mode
	 */
	model::FixMode fixMode() const;

	/**
	 * @brief      Get latitude and longitude values
	 */
	std::pair<double, double> location() const;

	/**
	 * @brief      Get latitude value
	 */
	double latitude() const;

	/**
	 * @brief      Get longitude value
	 */
	double longitude() const;

	/**
	 * @brief      Get altitude value
	 */
	double altitude() const;

	/**
	 * @brief      Get speed value
	 */
	float speed() const;

	/**
	 * @brief      Get bearing value
	 */
	float bearing() const;

	/**
	 * @brief      Get accuracy value
	 */
	float accuracy() const;

	/**
	 * @brief      Get timestamp value
	 */
	GpsUtcTime timestamp() const;

	/**
	 * @brief      Set the fix quality
	 */
	FixQuality quality(FixQuality value);

	/**
	 * @brief      Set the fix mode
	 */
	model::FixMode fixMode(model::FixMode value);

	/**
	 * @brief      Set and get latitude and longitude value
	 */
	std::pair<double, double> location(double latitude, double longitude);

	/**
	 * @brief      Set and get altitude value
	 */
	double altitude(double value);

	/**
	 * @brief      Set and get speed value
	 */
	float speed(float value);

	/**
	 * @brief      Set and get bearing value
	 */
	float bearing(float value);

	/**
	 * @brief      Set and get accuracy value
	 */
	float accuracy(float value);

	/**
	 * @brief      Set and get timestamp value
	 */
	GpsUtcTime timestamp(GpsUtcTime value);

	/**
	 * @brief      Get pointer to the Android platform location structure
	 */
	void copyToGpsLocation(GpsLocation & loc) const;

	/**
	 * @brief      Returns a string representation of the location.
	 *
	 * @return     String representation of the location.
	 */
	std::string toString() const;
};

}

#endif // TESEO_HAL_MODEL_LOCATION