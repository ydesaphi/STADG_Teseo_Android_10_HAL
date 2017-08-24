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
 * @file AbstractDecoder.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_DECODER_ABSTRACT_DECODER_H
#define TESEO_HAL_DECODER_ABSTRACT_DECODER_H

#include <teseo/utils/ByteVector.h>
#include <teseo/utils/Thread.h>
#include <teseo/utils/Channel.h>
#include <teseo/utils/Signal.h>

namespace stm {
namespace decoder {

/**
 * @brief      Abstract message decoder
 * 
 * @details    The abstract decoder receive bytes from the stream and decode it in its own decoding
 * task. The decoding is done by AbstractDecoder child class like NmeaDecoder.
 */
class AbstractDecoder:
	public Trackable,
	public Thread
{
private:
	thread::Channel<ByteVector *> bytesChannel;

	bool stopDecoder;

protected:
	/**
	 * @brief      Decoding task
	 */
	virtual void run();

	/**
	 * @brief      Decode bytes
	 *
	 * @param[in]  bytes  The bytes to decode
	 */
	virtual void decode(ByteVectorPtr bytes) = 0;

public:
	AbstractDecoder();

	virtual ~AbstractDecoder();

	/**
	 * @brief      New bytes available slot
	 * 
	 * @details    { detailed_item_description }
	 *
	 * @param[in]  bytes  The bytes to receive
	 */
	virtual void onNewBytes(ByteVectorPtr bytes);

	/**
	 * @brief      Stop the decoder thread
	 *
	 * @return     0 on success, 1 on failure
	 */
	virtual int stop();
};

} // namespace decoder
} // namespace stm

#endif // TESEO_HAL_DECODER_I_DECODER_H