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
#ifndef TESEO_HAL_UTILS_IBYTESTREAM_H
#define TESEO_HAL_UTILS_IBYTESTREAM_H

#include <stdexcept>
#include "Signal.h"
#include "ByteVector.h"
#include "Thread.h"
#include "Channel.h"

namespace stm {
namespace stream {

enum class ByteStreamStatus {
	OPENED,
	CLOSED,
	ERROR
};


class StreamException : public std::exception
{
public:
	enum Type : uint8_t {
		UNKNOWN = 0,
		OPEN  = 1,
		CLOSE = 2,
		READ  = 3,
		WRITE = 4,
		NOT_OPENED = 5
	};

	StreamException() noexcept :
		type(UNKNOWN)
	{ }

    explicit StreamException(Type t) noexcept:
		type(t)
	{ }

    StreamException(const StreamException & other) noexcept :
		type(other.type)
	{ }

    StreamException & operator=(const StreamException & other) noexcept
	{
		type = other.type;
	}

    virtual ~StreamException() noexcept
	{ }

    virtual const char* what() const noexcept
	{
		static const char * whats[] = {
			"Unknown stream error",
			"Stream open error",
			"Stream close error",
			"Stream read error",
			"Stream write error",
			"Stream isn't open"
		};

		return whats[static_cast<uint8_t>(type)];
	}

	virtual Type getType() const noexcept
	{
		return type;
	}

private:
	Type type;
};

#define STREAM_EXCEPTION(__NAME, __TYPE)                                 \
class __NAME : public StreamException                                    \
{                                                                        \
public: __NAME() noexcept : StreamException(StreamException::__TYPE) { } \
};

// Stream exception child classes:
STREAM_EXCEPTION(StreamOpenException,      OPEN)
STREAM_EXCEPTION(StreamCloseException,     CLOSE)
STREAM_EXCEPTION(StreamReadException,      READ)
STREAM_EXCEPTION(StreamWriteException,     WRITE)
STREAM_EXCEPTION(StreamNotOpenedException, NOT_OPENED)

#undef STREAM_EXCEPTION

template<bool CatchException>
class ByteStreamOpener;

class ByteStreamReader;
class ByteStreamWrite;

class IByteStream : public Trackable {
protected:
	template<bool CatchException>
	friend class ByteStreamOpener;
	
	friend class ByteStreamReader;
	friend class ByteStreamWriter;

	/**
	 * @brief Open device for reading and writing
	 */
	virtual void open() throw(StreamException) = 0;

	/**
	 * @brief Close device
	 */
	virtual void close() throw(StreamException) = 0;

	/**
	 * @brief Flush device
	 */
	virtual void flush() throw(StreamException) = 0;

	/**
	 * Read data from device
	 *
	 * @return Vector of bytes read
	 */
	virtual ByteVector perform_read() throw(StreamException)  = 0;

	virtual void perform_write(const ByteVectorPtr bytes) throw(StreamException) = 0;

public:
	virtual ~IByteStream() { }

	/**
	 * @brief Get the device name
	 *
	 * @return Return the device name
	 */
	virtual const std::string & name() const = 0;

	/**
	 * @brief Get current stream status
	 */
	virtual ByteStreamStatus status() const = 0;

	/**
	 * Write data to device
	 *
	 * @param data Data to write to device
	 * @param size Number of bytes to write
	 */
	virtual void write(const ByteVectorPtr bytes) = 0;

	/**
	 * Start asynchronous read/write
	 */
	virtual int start() = 0;

	/**
	 * Stop asynchronous read/write
	 */
	virtual int stop() = 0;

	/**
	 * New bytes signal
	 */
	Signal<void, const ByteVector &> newBytes;
};

namespace __private_ByteStreamOpenerLog {
void loge(const char * format, std::string streamName, const char * what);
} // namespace __private_ByteStreamOpenerLog

template<bool CatchException = false>
class ByteStreamOpener {
private:
	IByteStream & stream;
	bool closeOnDestroy;

public:
	ByteStreamOpener(IByteStream & s) :
		stream(s)
	{
		if(stream.status() != ByteStreamStatus::OPENED)
		{
			if(CatchException)
			{
				try
				{
					stream.open();
					closeOnDestroy = true;
				}
				catch(const StreamException & ex)
				{
					__private_ByteStreamOpenerLog::loge("Error while opening stream '%s': %s",
						stream.name(),
						ex.what());
				}
			}
			else
			{
				stream.open();
				closeOnDestroy = true;
			}
		}
		else
		{
			closeOnDestroy = false;
		}
	}

	operator bool ()
	{
		return stream.status() == ByteStreamStatus::OPENED;
	}

	~ByteStreamOpener()
	{
		if(closeOnDestroy)
		{
			if(CatchException)
			{
				try
				{
					stream.close();
				}
				catch(const StreamException & ex)
				{
					__private_ByteStreamOpenerLog::loge("Error while closing stream '%s': %s",
						stream.name(),
						ex.what());
				}
			}
			else
			{
				stream.close();
			}
		}
	}
};

class ByteStreamReader : public Thread {
protected:
	void run();

	IByteStream & byteStream;

	bool runReader;

public:
	ByteStreamReader(IByteStream & bs);

	int stop();
};

class ByteStreamWriter : public Thread {
private:
	enum Commands {
		WRITE,
		STOP
	};
	
protected:
	void run();

	IByteStream & byteStream;

	thread::Channel<Commands> com;

	thread::Channel<const ByteVectorPtr> dataChannel;

public:
	ByteStreamWriter(IByteStream & bs);

	int stop();

	void write(const ByteVectorPtr bytes);
};

class AbstractByteStream : public IByteStream {
private:
	ByteStreamReader reader;

	ByteStreamWriter writer;

public:
	AbstractByteStream();

	virtual ~AbstractByteStream();

	void write(const ByteVectorPtr bytes);

	int start();

	int stop();
};

} // namespace stream
} // namespace stm

#endif // TESEO_HAL_UTILS_IBYTESTREAM_H