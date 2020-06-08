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
#ifndef TESEO_HAL_UTILS_UARTBYTESTREAM_H
#define TESEO_HAL_UTILS_UARTBYTESTREAM_H

#include <iostream>
#include <fstream>

#include "IByteStream.h"
#include "Thread.h"
#include "DebugOutputStream.h"

namespace stm {
namespace stream {

class UartByteStream : public AbstractByteStream {
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
	 * TTY speed
	 */
	unsigned int speedDevice;

	/**
	 * Stream status
	 */
	ByteStreamStatus streamStatus;

	debug::DebugOutputStream dbgRx;
	debug::DebugOutputStream dbgTx;

	unsigned int openCount;

	std::mutex openMutex;

protected:

	/**
	 * @brief Open device for reading and writing
	 */
	virtual void open() noexcept(false);

	/**
	 * @brief Close device
	 */
	virtual void close() noexcept(false);

	/**
	 * @brief Flush the device
	 */
	virtual void flush() noexcept(false);

	/**
	 * Read data from device
	 *
	 * @return Vector of bytes read
	 */
	virtual ByteVector perform_read() noexcept(false);

	virtual void perform_write(const ByteVectorPtr bytes) noexcept(false);

public:
	explicit UartByteStream(const std::string& ttyDevice, unsigned int speedDevice);

	virtual ~UartByteStream();

	virtual const std::string& name() const;

	virtual ByteStreamStatus status() const;
};

} // namespace stream
} // namespace stm

#endif // TESEO_HAL_UTILS_IBYTESTREAM_H