/**
 * @file NmeaStream.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/utils/NmeaStream.h>

#define LOG_TAG "teseo_hal_NmeaStream"
#include <cutils/log.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <teseo/utils/errors.h>
#include <teseo/utils/Wakelock.h>

namespace stm {
namespace stream {

NmeaStream::NmeaStream(IByteStream & bs) :
	IStream(), Thread("teseo-nmea-stream"),
	byteStream(bs)
{
	continueReading = false;
}

NmeaStream::~NmeaStream()
{ }

void append(ByteVector & buffer, ByteVector::iterator first, ByteVector::iterator last)
{
	for(auto it = first; it != last; ++it)
	{
		uint8_t tmp = *it;

		if(tmp != '\r' && tmp != '\n' && tmp != '\0')
			buffer.push_back(tmp);
	}
}

void NmeaStream::run()
try
{
	ByteStreamOpener bsOpener(byteStream);

	ByteVector buffer;
	int errcount = 0;
	buffer.reserve(255);

	continueReading = true;

	ALOGI("Start NMEA stream reader");
	utils::Wakelock::acquire();

	byteStream.flush();

	while(continueReading)
	{
		ByteVector bytes = byteStream.read();

		if(bytes.size() > 0)
		{
			auto start = bytes.begin();
			auto bytesEnd = bytes.end();

			/*
			 * This for loop is responsible of splitting the stream at each $.
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
			for(auto it = start; it != bytesEnd; ++it)
			{
				if(*it == '$')
				{
					auto end = it;

					// Remove any \n or \r just before the dollar
					if(*(end - 1) == '\r' || *(end - 1) == '\n')
						end--;

					if(*(end - 1) == '\r' || *(end - 1) == '\n')
						end--;

					// Append data to buffer
					if(start < end)
						buffer.insert(buffer.end(), start, end);

					// Send and clear buffer
					onNewBytes(ByteVectorPtr(new ByteVector(buffer.begin(), buffer.end())));
					buffer.clear();

					// Set start to dollar position
					start = it;
				}
			}

			// Append the rest of the readed bytes to the buffer
			if(start < bytesEnd)
				buffer.insert(buffer.end(), start, bytesEnd);
		}
		else if(bytes.size() == 0)
		{
			ALOGW("0 bytes read on UART %s", byteStream.name().c_str());
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

	utils::Wakelock::release();
	ALOGI("End of NMEA stream reader.");
}
catch(const StreamException & ex)
{
	ALOGE("Stream exception during NMEA read task");
	ALOGE("Error: %s", ex.what());
}

void NmeaStream::write(const ByteVector & bytes)
try
{
	ByteStreamOpener bsOpener(byteStream);
	byteStream.write(bytes);
	byteStream.write({'\r', '\n'});
}
catch(const StreamException & ex)
{
	ALOGE("Stream exception during NMEA write task");
	ALOGE("Error: %s", ex.what());
}

void NmeaStream::write(const uint8_t * data, std::size_t size)
try
{
	ByteStreamOpener bsOpener(byteStream);
	byteStream.write(data, size);
	byteStream.write({'\r', '\n'});
}
catch(const StreamException & ex)
{
	ALOGE("Stream exception during NMEA write task");
	ALOGE("Error: %s", ex.what());
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