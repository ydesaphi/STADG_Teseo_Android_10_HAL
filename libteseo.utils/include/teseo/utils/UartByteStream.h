#ifndef TESEO_HAL_UTILS_UARTBYTESTREAM_H
#define TESEO_HAL_UTILS_UARTBYTESTREAM_H

#include "IByteStream.h"

namespace stm {
namespace stream {

class UartByteStream : public IByteStream {
private:
	/**
	 * TTY device file descriptor
	 */
	int fd;

	/**
	 * TTY device name
	 */
	std::string ttyDevice;

	/**
	 * Stream status
	 */
	ByteStreamStatus streamStatus;

public:
	explicit UartByteStream(const std::string& ttyDevice);

	virtual ~UartByteStream();

	virtual const std::string& name() const;

	virtual ByteStreamStatus status() const;

	/**
	 * @brief Open device for reading and writing
	 */
	virtual void open() throw(StreamException);

	/**
	 * @brief Close device
	 */
	virtual void close() throw(StreamException);

	virtual void flush() throw(StreamException);

	/**
	 * Read data from device
	 *
	 * @return Vector of bytes read
	 */
	virtual ByteVector read() throw(StreamException);

	/**
	 * Write data to device
	 *
	 * @param data Data to write to device
	 */
	virtual void write(const ByteVector& data) throw(StreamException);

	/**
	 * Write data to device
	 *
	 * @param data Data to write to device
	 * @param size Number of bytes to write
	 */
	virtual void write(const uint8_t * data, std::size_t size) throw(StreamException);
};

} // namespace stream
} // namespace stm

#endif // TESEO_HAL_UTILS_IBYTESTREAM_H