/**
 * @file NmeaStream.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include "NmeaStream.h"

#define LOG_TAG "teseo_hal_NmeaStream"
#include <cutils/log.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include "../errors.h"
#include "../LocServiceProxy.h"

namespace stm {
namespace stream {

NmeaStream::NmeaStream(const char * ttyDevice) :
	IStream(), Thread("teseo-nmea-stream"),
	ttyDevice(ttyDevice)
{
	continueReading = false;
}

NmeaStream::~NmeaStream()
{
	close();
}

void NmeaStream::open()
{
	// Open TTY device
	fd = ::open(ttyDevice.c_str(), O_RDWR | O_NOCTTY);
	CHECK_ERROR(fd, errors::open, "UART %s opened successfully.", ttyDevice.c_str());

	// Get current device attributes
	struct termios attr;
	tcgetattr(fd, &attr);

	// Set input/output baudrate
	cfsetispeed(&attr, B115200);
	cfsetospeed(&attr, B115200);

	// Disable stream modifications by kernel
	cfmakeraw(&attr);

	// CREAD => enable receiver
	// CLOCAL => local mode
	attr.c_cflag |= (CLOCAL | CREAD);

	// Apply new attributes
	tcsetattr(fd, TCSANOW, &attr);
}

void NmeaStream::close()
{
	if(fd != -1)
		CHECK_ERROR(::close(fd), errors::close, "UART %s closed successfully.", ttyDevice.c_str());
}

void append(ByteVector & buffer, uint8_t * first, uint8_t * last)
{
	for(auto it = first; it != last; ++it)
	{
		uint8_t tmp = *it;

		if(tmp != '\r' && tmp != '\n' && tmp != '\0')
			buffer.push_back(tmp);
	}
}

void NmeaStream::run()
{
	open();

	if(fd == -1)
	{
		ALOGE("UART %s is not opened, can't start reading it.", ttyDevice.c_str());
		return;
	}

	uint8_t bytes[256] = {0};
	ByteVector buffer;
	int errcount = 0;

	buffer.reserve(255);

	continueReading = true;

	ALOGI("Start NMEA stream reader");
	LocServiceProxy::gps::acquireWakelock();

	tcflush(fd, TCIOFLUSH);

	while(continueReading)
	{
		size_t nbBytesRead = read(fd, &bytes, 255);

		if(nbBytesRead > 0)
		{
			uint8_t * start = bytes;
			uint8_t * bytesEnd = bytes + nbBytesRead;

			/*
			 * This for is responsible of splitting the stream at each $.
			 * 
			 * When a $ is found its position is store in end, then we remove any trailing '\r' or
			 * '\n'.
			 * 
			 * If start is below end, the data from start (included) to end (excluded) is appended
			 * to buffer.
			 * 
			 * The buffer is sent for processing, then cleared.
			 * 
			 * The new frame start is set at dollar position.
			 */
			for(uint8_t * it = start; it != bytesEnd; ++it)
			{
				if(*it == '$')
				{
					uint8_t * end = it;

					// Remove any \n or \r just before the dollar
					if(*(end - 1) == '\r' || *(end - 1) == '\n')
						end--;

					if(*(end - 1) == '\r' || *(end - 1) == '\n')
						end--;

					// Append data to buffer
					if(start < end)
						append(buffer, start, end);

					// Send and clear buffer
					onNewBytes(ByteVectorPtr(new ByteVector(buffer.begin(), buffer.end())));
					buffer.clear();

					// Set start to dollar position
					start = it;
				}
			}

			if(start < bytesEnd)
				append(buffer, start, bytesEnd);
		}
		else if(nbBytesRead == 0)
		{
			ALOGW("0 bytes read on UART %s", ttyDevice.c_str());
		}
		else
		{
			errors::read(errno);

			if(errcount++ > 10)
			{
				ALOGE("Too much errors, stop reading.");
				continueReading = false;
			}
		}
	}

	close();

	LocServiceProxy::gps::releaseWakelock();
	ALOGI("End of NMEA stream reader.");
}

void NmeaStream::write(const std::vector<uint8_t> & bytes)
{
	ALOGW("NmeaStream::write Not implemented.");
	(void)(bytes);
}

int NmeaStream::startReading()
{
	return start();
}

int NmeaStream::stopReading()
{
	return stop();
}

int NmeaStream::stop()
{
	continueReading = false;
	return 0;
}

} // namespace stream
} // namespace stm