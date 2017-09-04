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
#include <teseo/utils/UartByteStream.h>

#include <stdexcept>

//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

#define LOG_TAG "teseo_hal_UartByteStream"
#include <cutils/log.h>

#include <teseo/config/config.h>
#include <teseo/utils/errors.h>

#define UART_BYTE_STREAM_BUFFER_SIZE 255

namespace stm {
namespace stream {

UartByteStream::UartByteStream(const std::string & ttyDevice) :
	AbstractByteStream(),
	fd(-1),
	ttyDevice(ttyDevice),
	streamStatus(ByteStreamStatus::CLOSED),
	dbgRx(13370),
	dbgTx(13371),
	openCount(0)
{
	dbgRx.start();
	dbgTx.start();
}

UartByteStream::~UartByteStream()
{
	dbgRx.stop();
	dbgTx.stop();

	if(streamStatus == ByteStreamStatus::OPENED)
	{
		try
		{
			close();
		}
		catch(const StreamException & ex)
		{
			ALOGE("Error while close UART Byte Stream");
			ALOGE("Error: %s", ex.what());
		}
	}
}

const std::string & UartByteStream::name() const
{
	return ttyDevice;
}

ByteStreamStatus UartByteStream::status() const
{
	return streamStatus;
}

void UartByteStream::open() throw(StreamException)
{
	// Because we use a open count we must synchronize access to open
	std::unique_lock<std::mutex> lock(openMutex);

	if(streamStatus == ByteStreamStatus::OPENED)
	{
		ALOGW("UART %s already opened.", ttyDevice.c_str());
		ALOGV("Increment UART open count");
		openCount++;
		return;
	}

	// Open TTY device
	fd = ::open(ttyDevice.c_str(), O_RDWR | O_NOCTTY);
	CHECK_ERROR(fd, errors::open, "UART %s opened successfully.", ttyDevice.c_str());

	if(fd == -1)
	{
		streamStatus = ByteStreamStatus::ERROR;
		throw StreamOpenException();
	}

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

	streamStatus = ByteStreamStatus::OPENED;

	flush();
	openCount++;
}

void UartByteStream::flush() throw(StreamException)
{
	if(streamStatus == ByteStreamStatus::OPENED)
	{
		tcflush(fd, TCIOFLUSH);
	}
	else
	{
		throw StreamNotOpenedException();
	}
}

void UartByteStream::close() throw(StreamException)
{
	// Because we use a open count we must synchronize access to close
	std::unique_lock<std::mutex> lock(openMutex);

	if(streamStatus == ByteStreamStatus::OPENED && openCount != 0)
	{
		if(openCount > 1)
		{
			ALOGV("Decrement UART Byte Stream open count");
			openCount--;
		}
		else
		{
			auto ret = ::close(fd);
			CHECK_ERROR(ret, errors::close, "UART %s closed successfully.", ttyDevice.c_str());

			fd = -1;
			openCount--;

			if(ret == -1)
			{
				streamStatus = ByteStreamStatus::ERROR;
				throw StreamException(StreamException::CLOSE);
			}
			else
			{
				streamStatus = ByteStreamStatus::CLOSED;
			}
		}
	}
	else
	{
		ALOGW("UART %s isn't opened.", ttyDevice.c_str());
	}
}

ByteVector UartByteStream::perform_read() throw(StreamException)
{
	if(streamStatus == ByteStreamStatus::OPENED)
	{
		uint8_t bytes[UART_BYTE_STREAM_BUFFER_SIZE + 1] = {0};
		ByteVector output;
		ssize_t nbBytes = 0;

		nbBytes = ::read(fd, &bytes, UART_BYTE_STREAM_BUFFER_SIZE);

		if(nbBytes == -1)
		{
			errors::read(errno);
			throw StreamException(StreamException::READ);
		}

		output.reserve(nbBytes);

		for(ssize_t i = 0; i < nbBytes; i++)
			output.push_back(bytes[i]);

		dbgRx.send(output);
		return output;
	}
	else
	{
		ALOGW("Can't read from closed UART");
		throw StreamException(StreamException::NOT_OPENED);
	}
}

void UartByteStream::perform_write(const ByteVectorPtr bytes) throw(StreamException)
{
	if(streamStatus == ByteStreamStatus::OPENED)
	{
		dbgTx.send(bytes->data(), bytes->size());
		ssize_t nbBytes = ::write(fd, bytes->data(), bytes->size());

		if(nbBytes == -1)
		{
			errors::write(errno);
			throw StreamException(StreamException::WRITE);
		}

		if(static_cast<std::size_t>(nbBytes) != bytes->size())
			ALOGW("Incoherent number of bytes written: %zd != bytes->size() == %zd",
				nbBytes, bytes->size());
	}
	else
	{
		ALOGE("UART isn't opened, can't write to it.");
		throw StreamException(StreamException::NOT_OPENED);
	}
}

} // namespace stream
} // namespace stm