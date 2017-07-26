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