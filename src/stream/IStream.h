/**
 * @file IStream.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_I_STREAM_H
#define TESEO_HAL_I_STREAM_H

#include <memory>
#include <vector>
#include <cstdint>

#include "Signal.h"
#include "../utils/ByteVector.h"

namespace stm {
namespace stream {

/**
 * @brief      Byte stream interface
 * 
 * @details    A byte stream supports the following operations:
 * - Asynchronous read
 * - Synchronous write
 * 
 * To read a stream you must connect a slot to the `onNewBytes` signal. Then you start the reading
 * task using the `startReading()` method.
 */
class IStream {
public:
	IStream() : onNewBytes("IStream::onNewBytes") { }

	virtual ~IStream() { }

	/**
	 * @brief      Starts the reading task.
	 *
	 * @return     0 on success, 1 on failure.
	 */
	virtual int startReading() = 0;

	/**
	 * @brief      Stops the reading task.
	 * 
	 * @details    This method only issue a stop request. You may call join to wait until reading
	 * task completion.
	 *
	 * @return     0 on success, 1 on failure.
	 */
	virtual int stopReading() = 0;

	/**
	 * Signal emitted when new bytes are available
	 */
	Signal<void, ByteVectorPtr> onNewBytes;

	/**
	 * @brief      Write bytes to the stream
	 *
	 * @param[in]  bytes  The bytes
	 */
	virtual void write(const ByteVector & bytes) = 0;
};

} // namespace stream
} // namespace stm

#endif // TESEO_HAL_I_STREAM_H