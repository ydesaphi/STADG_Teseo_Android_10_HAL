/**
 * @file NmeaStream.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_NMEA_STREAM_H
#define TESEO_HAL_NMEA_STREAM_H

#include <string>

#include "IStream.h"
#include "../thread/Thread.h"
#include "../Signal.h"

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
	/**
	 * TTY device file descriptor
	 */
	int fd;

	/**
	 * TTY device name
	 */
	std::string ttyDevice;

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
	 * @brief      Open the TTY device
	 */
	void open();

	/**
	 * @brief      Close the TTY device
	 */
	void close();

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
	NmeaStream(const char * ttyDevice);

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
};

} // namespace stream
} // namespace stm

#endif // TESEO_HAL_NMEA_STREAM_H