/**
 * @file AbstractDecoder.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_DECODER_ABSTRACT_DECODER_H
#define TESEO_HAL_DECODER_ABSTRACT_DECODER_H

#include "../utils/ByteVector.h"
#include "../thread/Thread.h"
#include "../thread/Channel.h"
#include "../Signal.h"

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