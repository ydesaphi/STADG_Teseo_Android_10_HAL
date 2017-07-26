/**
 * @file NmeaStream.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_NMEA_STREAM_H
#define TESEO_HAL_NMEA_STREAM_H

#include <string>

#include <teseo/utils/Thread.h>
#include <teseo/utils/Signal.h>

#include "IStream.h"
#include "IByteStream.h"

namespace stm {
namespace stream {

/**
 * @brief      NMEA Stream reader/writer
 */
class NmeaStream :
	public IStream,
	public Thread,
	public Trackable
{
private:
	IByteStream & byteStream;

	/**
	 * Reading task running flag. Set to false to request task stop.
	 */
	bool continueReading;

	/**
	 * @brief      Reading task stop method
	 * 
	 * @details    This method overload the visibility of the Thread::stop method.
	 *
	 * @return     0 on success, 1 on failure
	 */
	virtual int stop();

protected:

	/**
	 * @brief      Reading task
	 */
	virtual void run();

public:
	/**
	 * @brief      Open an NMEA stream on ttyDevice
	 *
	 * @param[in]  ttyDevice  The tty device
	 */
	NmeaStream(IByteStream & byteStream);

	/**
	 * @brief      NMEA Stream desctructor, closes the tty device
	 */
	virtual ~NmeaStream();

	/**
	 * @brief      Starts the reading task.
	 *
	 * @return     0 on success, 1 on failure.
	 */
	virtual int startReading();

	/**
	 * @brief      Stops the reading task.
	 * 
	 * @details    This method only issue a stop request. You may call join to wait until reading
	 * task completion.
	 *
	 * @return     0 on success, 1 on failure.
	 */
	virtual int stopReading();

	/**
	 * @brief      Write data to the NMEA stream
	 *
	 * @param[in]  bytes  The bytes
	 */
	virtual void write(const ByteVector & bytes);

	/**
	 * @brief      Write data to the NMEA stream
	 *
	 * @param[in]  data  The bytes
	 * @param[in]  size  The number of bytes
	 */
	virtual void write(const uint8_t * data, std::size_t size);
};

} // namespace stream
} // namespace stm

#endif // TESEO_HAL_NMEA_STREAM_H