/**
 * @file Location.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_MODEL_LOCATION
#define TESEO_HAL_MODEL_LOCATION

#include <hardware/gps.h>
#include <teseo/utils/ByteVector.h>

namespace stm {

/**
 * @brief      Location class
 */
class Location {
private:

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

	Location(GpsUtcTime timestamp, double lat, double lon);

	Location(GpsUtcTime timestamp, double lat, double lon, double alt);

	Location(GpsUtcTime timestamp, double lat, double lon, double alt, float speed);

	Location(GpsUtcTime timestamp, double lat, double lon, double alt, float speed, float bearing);

	Location(
		GpsUtcTime timestamp,
		double lat,
		double lon,
		double alt,
		float speed,
		float bearing,
		float accuracy);

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