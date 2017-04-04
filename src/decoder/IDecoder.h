#ifndef TESEO_HAL_DECODER_I_DECODER_H
#define TESEO_HAL_DECODER_I_DECODER_H

#include "../utils/ByteVector.h"
#include "../thread/Thread.h"
#include "../thread/Channel.h"
#include "../Signal.h"

namespace stm {
namespace decoder {

class IDecoder : 
	public Trackable,
	public Thread
{
private:

	thread::Channel<ByteVector *> bytesChannel;

	bool stopDecoder;

protected:
	virtual void run();

	virtual void decode(ByteVectorPtr bytes) = 0;

public:
	IDecoder();

	virtual ~IDecoder();

	virtual void onNewBytes(ByteVectorPtr bytes);

	virtual int stop();
};

} // namespace decoder
} // namespace stm

#endif // TESEO_HAL_DECODER_I_DECODER_H