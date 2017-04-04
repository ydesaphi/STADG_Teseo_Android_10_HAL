#ifndef TESEO_HAL_ABSTRACT_DEVICE_H
#define TESEO_HAL_ABSTRACT_DEVICE_H

#include <mutex>
#include <hardware/gps.h>

#include "../Signal.h"
#include "../model/NmeaMessage.h"
#include "../model/Location.h"
#include "../decoder/IDecoder.h"
#include "../stream/IStream.h"
#include "../thread/Thread.h"

namespace stm {
namespace decoder {
	class NmeaDecoder;
	namespace nmea {
		struct decoders;
	} // namespace nmea
} // namespace decoder

namespace device {

class AbstractDevice :
	public Trackable
{
private:
	GpsUtcTime timestamp;

	stream::IStream * stream;

	decoder::IDecoder * decoder;

	Location location;

protected:

	// Allow NmeaDecoder to use emitNmea
	friend class stm::decoder::NmeaDecoder;

	// Allow decoding functions to update device data model
	friend struct stm::decoder::nmea::decoders;

	AbstractDevice();

	void connectStreamToDecoder();

	void setStream(stream::IStream * s);

	void setDecoder(decoder::IDecoder * d);

	void setTimestamp(GpsUtcTime timestamp);

	void invalidateLocation();

	void invalidateAltitude();

	void invalidateSpeed();

	void invalidateBearing();

	void invalidateAccuracy();

	void setLocation(double latitude, double longitude);

	void setAltitude(double altitude);

	void setSpeed(float speed);

	void setBearing(float bearing);

	void setAccuracy(float accuracy);

	void emitNmea(const NmeaMessage & nmea);

	void update();

public:
	virtual ~AbstractDevice() { }

	GpsUtcTime getTimestamp() const { return timestamp; }

	const Location & getLocation() const { return location; }

	int start();

	int stop();

	Signal<void, GpsUtcTime, const NmeaMessage &> onNmea;

	Signal<void, Location &> locationUpdate;
};

} // namespace device
} // namespace stm

#endif // TESEO_HAL_ABSTRACT_DEVICE_H