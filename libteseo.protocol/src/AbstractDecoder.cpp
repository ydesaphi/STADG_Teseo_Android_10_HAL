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
 * @file AbstractDecoder.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/protocol/AbstractDecoder.h>

#define LOG_TAG "teseo_hal_AbstractDecoder"
#include <cutils/log.h>
#include <stdexcept>

#include <teseo/utils/errors.h>
#include <teseo/utils/Wakelock.h>

namespace stm {
namespace decoder {

AbstractDecoder::AbstractDecoder() :
	Trackable(),
	Thread("teseo-decoder"),
	bytesChannel("AbstractDecoder::bytesChannel")
{
	stopDecoder = false;
}

AbstractDecoder::~AbstractDecoder()
{ }

void AbstractDecoder::run()
{
	ByteVector * bytes = nullptr;
	int errcount = 0;

	stopDecoder = false;

	ALOGI("Start decoder thread");
	utils::Wakelock::acquire();

	while(!stopDecoder)
	{
		try
		{
			bytes = bytesChannel.receive();

			if(bytes != nullptr)
				decode(ByteVectorPtr(bytes));
			else
				ALOGW("Received nullptr, thread should stop shortly.");
		}
		catch(const std::system_error & ex)
		{
			if(errcount++ > 10)
			{
				ALOGE("Too much channel errors, stop decoding.");
				stopDecoder = true;
			}
		}
		catch(const std::runtime_error & ex)
		{
			if(errcount++ > 10)
			{
				ALOGE("Too much channel errors, stop decoding.");
				stopDecoder = true;
			}
		}
	}

	utils::Wakelock::release();
	ALOGI("End of decoder thread");
}

void AbstractDecoder::onNewBytes(ByteVectorPtr bytes)
{
	if(isRunning())
	{
		bytesChannel.send(new ByteVector(*bytes));
	}
	else
	{
		ALOGW("Bytes ignored because decoder is stopped.");
	}
}

int AbstractDecoder::stop()
{
	if(isRunning())
	{
		ALOGI("Stop decoder thread");

		stopDecoder = true;

		bytesChannel.send(nullptr);

		return 0;
	}
	else
	{
		ALOGW("Decoder thread is already stopped.");
		return -1;
	}
}

} // namespace decoder
} // namespace stm