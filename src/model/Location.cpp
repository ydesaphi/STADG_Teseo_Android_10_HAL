#include "Location.h"
#include "string.h"

#include <sstream>

namespace stm {

Location::Location()
{
	loc.flags = 0;
}

Location::Location(const Location & other)
{
	memcpy(&loc, &(other.loc), sizeof(GpsLocation));
}

Location::Location(Location && other)
{
	memcpy(&loc, &(other.loc), sizeof(GpsLocation));
}

Location::Location(GpsUtcTime timestamp, double lat, double lon)
{
	loc.timestamp = timestamp;
	loc.flags = GPS_LOCATION_HAS_LAT_LONG;
	loc.latitude = lat;
	loc.longitude = lon;
}

Location::Location(GpsUtcTime timestamp, double lat, double lon, double alt)
{
	loc.timestamp = timestamp;
	loc.flags = GPS_LOCATION_HAS_LAT_LONG | GPS_LOCATION_HAS_ALTITUDE;
	loc.latitude = lat;
	loc.longitude = lon;
	loc.altitude = alt;
}

Location::Location(GpsUtcTime timestamp, double lat, double lon, double alt, float speed)
{
	loc.timestamp = timestamp;
	loc.flags = GPS_LOCATION_HAS_LAT_LONG | GPS_LOCATION_HAS_ALTITUDE | GPS_LOCATION_HAS_SPEED;
	loc.latitude = lat;
	loc.longitude = lon;
	loc.altitude = alt;
	loc.speed = speed;
}

Location::Location(
	GpsUtcTime timestamp,
	double lat,
	double lon,
	double alt,
	float speed,
	float bearing)
{
	loc.timestamp = timestamp;
	loc.flags = GPS_LOCATION_HAS_LAT_LONG |
	            GPS_LOCATION_HAS_ALTITUDE |
	            GPS_LOCATION_HAS_SPEED    |
	            GPS_LOCATION_HAS_BEARING;
	loc.latitude = lat;
	loc.longitude = lon;
	loc.altitude = alt;
	loc.speed = speed;
	loc.bearing = bearing;
}

Location::Location(
	GpsUtcTime timestamp,
	double lat,
	double lon,
	double alt,
	float speed,
	float bearing,
	float accuracy)
{
	loc.timestamp = timestamp;
	loc.flags = GPS_LOCATION_HAS_LAT_LONG |
	            GPS_LOCATION_HAS_ALTITUDE |
	            GPS_LOCATION_HAS_SPEED    |
	            GPS_LOCATION_HAS_BEARING  |
	            GPS_LOCATION_HAS_ACCURACY;
	loc.latitude = lat;
	loc.longitude = lon;
	loc.altitude = alt;
	loc.speed = speed;
	loc.bearing = bearing;
	loc.accuracy = accuracy;
}

double Location::latitude() const
{
	return loc.latitude;
}

double Location::longitude() const
{
	return loc.longitude;
}

double Location::altitude() const
{
	return loc.altitude;
}

float Location::speed() const
{
	return loc.speed;
}

float Location::bearing() const
{
	return loc.bearing;
}

float Location::accuracy() const
{
	return loc.accuracy;
}

GpsUtcTime Location::timestamp() const
{
	return loc.timestamp;
}

double Location::latitude(double value)
{
	loc.flags |= GPS_LOCATION_HAS_LAT_LONG;
	loc.latitude = value;
	return loc.latitude;
}

double Location::longitude(double value)
{
	loc.flags |= GPS_LOCATION_HAS_LAT_LONG;
	loc.longitude = value;
	return loc.longitude;
}

double Location::altitude(double value)
{
	loc.flags |= GPS_LOCATION_HAS_ALTITUDE;
	loc.altitude = value;
	return loc.altitude;
}

float Location::speed(float value)
{
	loc.flags |= GPS_LOCATION_HAS_SPEED;
	loc.speed = value;
	return loc.speed;
}

float Location::bearing(float value)
{
	loc.flags |= GPS_LOCATION_HAS_BEARING;
	loc.bearing = value;
	return loc.bearing;
}

float Location::accuracy(float value)
{
	loc.flags |= GPS_LOCATION_HAS_ACCURACY;
	loc.accuracy = value;
	return loc.accuracy;
}

GpsUtcTime Location::timestamp(GpsUtcTime value)
{
	loc.timestamp = value;
	return loc.timestamp;
}

const GpsLocation * Location::getAndroidStruct() const
{
	return &loc;
}

GpsLocation * Location::getAndroidStruct()
{
	return &loc;
}

std::string Location::toString() const
{
	std::ostringstream buffer;

	buffer << "time = " << loc.timestamp << ", pos = [" << loc.latitude << ", " << loc.longitude
	       << "], alt = " << loc.altitude << ", speed = " << loc.speed
	       << ", bearing = " << loc.bearing << ", accuracy = " << loc.accuracy
	       << ", flags = 0x" << std::hex << loc.flags;

	return buffer.str();
}

bool Location::locationValidity() const
{
	return (loc.flags & GPS_LOCATION_HAS_LAT_LONG) != 0;
}

bool Location::altitudeValidity() const
{
	return (loc.flags & GPS_LOCATION_HAS_ALTITUDE) != 0;
}

bool Location::speedValidity() const
{
	return (loc.flags & GPS_LOCATION_HAS_SPEED) != 0;
}

bool Location::bearingValidity() const
{
	return (loc.flags & GPS_LOCATION_HAS_BEARING) != 0;
}

bool Location::accuracyValidity() const
{
	return (loc.flags & GPS_LOCATION_HAS_ACCURACY) != 0;
}

void Location::invalidateLocation()
{
	loc.flags &= ~(GPS_LOCATION_HAS_LAT_LONG);
}

void Location::invalidateAltitude()
{
	loc.flags &= ~(GPS_LOCATION_HAS_ALTITUDE);
}

void Location::invalidateSpeed()
{
	loc.flags &= ~(GPS_LOCATION_HAS_SPEED);
}

void Location::invalidateBearing()
{
	loc.flags &= ~(GPS_LOCATION_HAS_BEARING);
}

void Location::invalidateAccuracy()
{
	loc.flags &= ~(GPS_LOCATION_HAS_ACCURACY);
}

} // namespace stm