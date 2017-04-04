#ifndef TESEO_HAL_MODEL_LOCATION
#define TESEO_HAL_MODEL_LOCATION

#include <hardware/gps.h>
#include "../utils/ByteVector.h"

namespace stm {

class Location {
private:
	GpsLocation loc;

public:
	Location();

	Location(const Location & other);

	Location(Location && other);

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

	bool locationValidity() const;

	bool altitudeValidity() const;

	bool speedValidity() const;

	bool bearingValidity() const;

	bool accuracyValidity() const;

	void invalidateLocation();

	void invalidateAltitude();

	void invalidateSpeed();

	void invalidateBearing();

	void invalidateAccuracy();

	double latitude() const;

	double longitude() const;

	double altitude() const;

	float speed() const;

	float bearing() const;

	float accuracy() const;

	GpsUtcTime timestamp() const;

	double latitude(double value);

	double longitude(double value);

	double altitude(double value);

	float speed(float value);

	float bearing(float value);

	float accuracy(float value);

	GpsUtcTime timestamp(GpsUtcTime value);

	const GpsLocation * getAndroidStruct() const;
	GpsLocation * getAndroidStruct();

	std::string toString() const;
};

}

#endif // TESEO_HAL_MODEL_LOCATION