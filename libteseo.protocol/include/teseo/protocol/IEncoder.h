#ifndef TESEO_HAL_PROTOCOL_IENCODER_H
#define TESEO_HAL_PROTOCOL_IENCODER_H

#include <teseo/model/Message.h>
#include <teseo/utils/Signal.h>

namespace stm {
namespace protocol {

class IEncoder : public Trackable {
public:
	virtual ~IEncoder() { }

	virtual void encode(model::MessageId id) = 0;

	Signal<void, const uint8_t *, std::size_t> encodedBytes;
};

} // namespace protocol
} // namespace stm

#endif // TESEO_HAL_PROTOCOL_IENCODER_H