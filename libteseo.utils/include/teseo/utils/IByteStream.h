#ifndef TESEO_HAL_UTILS_IBYTESTREAM_H
#define TESEO_HAL_UTILS_IBYTESTREAM_H

#include <stdexcept>
#include "ByteVector.h"

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

class IByteStream {
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
	virtual ByteVector read() throw(StreamException)  = 0;

	/**
	 * Write data to device
	 *
	 * @param data Data to write to device
	 */
	virtual void write(const ByteVector & data) throw(StreamException) = 0;

	virtual void write(const uint8_t * data, std::size_t size) throw(StreamException) = 0;
};

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
			stream.open();
			closeOnDestroy = true;
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
			stream.close();
	}
};

} // namespace stream
} // namespace stm

#endif // TESEO_HAL_UTILS_IBYTESTREAM_H