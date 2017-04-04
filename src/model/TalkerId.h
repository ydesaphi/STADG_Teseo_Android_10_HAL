#ifndef TESEO_HAL_MODEL_TALKER_ID_H
#define TESEO_HAL_MODEL_TALKER_ID_H

#include "../utils/ByteVector.h"

namespace stm {

enum class TalkerId : uint8_t {
	GP   = 0, ///< Global Positioning System (GPS)
	GA   = 1, ///< Galileo Positioning System
	GL   = 2, ///< GLONASS Receiver
	GN   = 3, ///< Global Navigation Satellite System (GNSS)
	GB   = 4, ///< Beidou
	BD   = 5, ///< Beidou
	QZ   = 6, ///< QZSS
	PSTM = 7, ///< Proprietary STM
	INVALID
};

const char * TalkerIdToString(TalkerId id);

TalkerId ByteVectorToTalkerId(const ByteVector & bytes);

} // namespace stm

#endif // TESEO_HAL_MODEL_TALKER_ID_H