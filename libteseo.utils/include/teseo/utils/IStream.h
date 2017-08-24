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

#include <teseo/utils/Signal.h>
#include <teseo/utils/ByteVector.h>

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
class IStream : public Trackable
{
public:
	IStream() : newSentence("IStream::newSentence") { }

	virtual ~IStream() { }

	virtual void onNewBytes(const ByteVector & bytes) = 0;

	/**
	 * Signal emitted when new bytes are available
	 */
	Signal<void, ByteVectorPtr> newSentence;

	/**
	 * Signal emitted when new bytes are ready to write
	 */
	Signal<void, const uint8_t *, std::size_t> newBytesToWrite; 

	/**
	 * @brief      Write bytes to the stream
	 *
	 * @param[in]  data   The bytes
	 * @param[in]  size   Number of bytes
	 */
	virtual void write(const uint8_t * data, std::size_t size) = 0;
};

} // namespace stream
} // namespace stm

#endif // TESEO_HAL_I_STREAM_H