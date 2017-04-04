#ifndef TESEO_HAL_I_STREAM_H
#define TESEO_HAL_I_STREAM_H

#include <memory>
#include <vector>
#include <cstdint>

#include "Signal.h"
#include "../utils/ByteVector.h"

namespace stm {
namespace stream {

class IStream {
public:
	IStream() : onNewBytes("IStream::onNewBytes") { }

	virtual ~IStream() { }

	virtual int startReading() = 0;

	virtual int stopReading() = 0;

	Signal<void, ByteVectorPtr> onNewBytes;

	virtual void write(const ByteVector & bytes) = 0;
};

} // namespace stream
} // namespace stm

#endif // TESEO_HAL_I_STREAM_H