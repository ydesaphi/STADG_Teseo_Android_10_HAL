#ifndef TESEO_HAL_NMEA_STREAM_H
#define TESEO_HAL_NMEA_STREAM_H

#include <string>

#include "IStream.h"
#include "../thread/Thread.h"
#include "../Signal.h"

namespace stm {
namespace stream {

class NmeaStream :
	public IStream,
	public Thread,
	public Trackable
{
private:
	int fd;

	std::string ttyDevice;

	bool continueReading;

	virtual int stop();

protected:
	void open();

	void close();

	virtual void run();

public:
	NmeaStream(const char * ttyDevice);

	virtual ~NmeaStream();

	virtual int startReading();

	virtual int stopReading();

	virtual void write(const ByteVector & bytes);
};

} // namespace stream
} // namespace stm

#endif // TESEO_HAL_NMEA_STREAM_H