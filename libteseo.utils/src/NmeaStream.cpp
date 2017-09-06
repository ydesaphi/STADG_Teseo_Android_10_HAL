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
 * @file NmeaStream.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/utils/NmeaStream.h>

#define LOG_TAG "teseo_hal_NmeaStream"
#include <cutils/log.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <teseo/utils/errors.h>
#include <teseo/utils/Wakelock.h>

namespace stm {
namespace stream {

NmeaStream::NmeaStream() :
	IStream()
{
	buffer.reserve(255);
}

NmeaStream::~NmeaStream()
{ }

void append(ByteVector & buffer, ByteVector::iterator first, ByteVector::iterator last)
{
	for(auto it = first; it != last; ++it)
	{
		uint8_t tmp = *it;

		if(tmp != '\r' && tmp != '\n' && tmp != '\0')
			buffer.push_back(tmp);
	}
}

void NmeaStream::onNewBytes(const ByteVector & bytes)
{
	if(bytes.size() > 0)
	{
		auto start = bytes.begin();
		auto bytesEnd = bytes.end();

		/*
			* This for loop is responsible of splitting the stream at each $.
			* 
			* When a $ is found its position is store in end, then we remove any trailing '\r' or
			* '\n'.
			* 
			* If start is below end, the data from start (included) to end (excluded) is appended
			* to buffer.
			* 
			* The buffer is sent for processing, then cleared.
			* 
			* The new frame start is set at dollar position.
			*/
		for(auto it = start; it != bytesEnd; ++it)
		{
			if(*it == '$')
			{
				auto end = it;

				// Remove any \n or \r just before the dollar
				if(*(end - 1) == '\r' || *(end - 1) == '\n')
					end--;

				if(*(end - 1) == '\r' || *(end - 1) == '\n')
					end--;

				// Append data to buffer
				if(start < end)
					buffer.insert(buffer.end(), start, end);
				
				// Send and clear buffer
				newSentence(ByteVectorPtr(new ByteVector(buffer.begin(), buffer.end())));
				buffer.clear();

				// Set start to dollar position
				start = it;
			}
		}

		// Append the rest of the readed bytes to the buffer
		if(start < bytesEnd)
			buffer.insert(buffer.end(), start, bytesEnd);
	}
	else if(bytes.size() == 0)
	{
		ALOGW("0 bytes received");
	}
}

void NmeaStream::write(ByteVectorPtr bytes)
{
	uint8_t crc = 0;
	std::string logStr;

	ByteVectorPtr toWritePtr = std::make_shared<ByteVector>();
	ByteVector & toWrite = *toWritePtr;

	toWrite.push_back('$');

	for(uint8_t b : *bytes)
	{
		crc ^= b;
		toWrite.push_back(b);
	}

	toWrite << '*' << utils::to_ascii(crc) << "\r\n";

	ALOGI("Out NMEA: '%s'", utils::bytesToString(toWrite).c_str());
	newBytesToWrite(toWritePtr);
}

} // namespace stream
} // namespace stm