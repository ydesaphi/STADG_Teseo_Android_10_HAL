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
 * @file NmeaStream.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_NMEA_STREAM_H
#define TESEO_HAL_NMEA_STREAM_H

#include <string>

#include <teseo/utils/ByteVector.h>
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
	public Trackable
{
private:
	ByteVector buffer;

	/**
	 * Reading task running flag. Set to false to request task stop.
	 */
	bool continueReading;

public:
	/**
	 * @brief      Open an NMEA stream on ttyDevice
	 *
	 * @param[in]  ttyDevice  The tty device
	 */
	NmeaStream();

	/**
	 * @brief      NMEA Stream desctructor, closes the tty device
	 */
	virtual ~NmeaStream();

	virtual void onNewBytes(const ByteVector & bytes);

	/**
	 * @brief      Write data to the NMEA stream
	 *
	 * @param[in]  data  The bytes
	 * @param[in]  size  The number of bytes
	 */
	virtual void write(ByteVectorPtr bytes);
};

} // namespace stream
} // namespace stm

#endif // TESEO_HAL_NMEA_STREAM_H