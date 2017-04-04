#include "IDecoder.h"

#define LOG_TAG "teseo_hal_IDecoder"
#include <cutils/log.h>
#include <stdexcept>

#include "../LocServiceProxy.h"
#include "../errors.h"

namespace stm {
namespace decoder {

IDecoder::IDecoder() :
	Trackable(),
	Thread("teseo-decoder"),
	bytesChannel("IDecoder::bytesChannel")
{
	stopDecoder = false;
}

IDecoder::~IDecoder()
{ }

void IDecoder::run()
{
	ByteVector * bytes = nullptr;
	int errcount = 0;

	stopDecoder = false;

	ALOGI("Start decoder thread");
	LocServiceProxy::gps::acquireWakelock();

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

	LocServiceProxy::gps::releaseWakelock();
	ALOGI("End of decoder thread");
}

void IDecoder::onNewBytes(ByteVectorPtr bytes)
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

int IDecoder::stop()
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