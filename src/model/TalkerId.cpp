/**
 * @file TalkerId.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include "TalkerId.h"

namespace stm {

const char * TalkerIdToString(TalkerId id)
{
	static const char * talkers[] = {
		"GP",
		"GA",
		"GL",
		"GN",
		"GB",
		"BD",
		"QZ",
		"PSTM",
		"INVALID"
	};

	return talkers[static_cast<uint8_t>(id)];
}


TalkerId ByteVectorToTalkerId(const ByteVector & bytes)
{
	if(bytes.size() < 2)
		return TalkerId::INVALID;

	uint8_t b = bytes[1];

	if(b == 'P') return TalkerId::GP;
	if(b == 'S') return TalkerId::PSTM;
	if(b == 'L') return TalkerId::GL;
	if(b == 'B') return TalkerId::GB;
	if(b == 'A') return TalkerId::GA;
	if(b == 'N') return TalkerId::GN;
	if(b == 'D') return TalkerId::BD;
	if(b == 'Z') return TalkerId::QZ;

	return TalkerId::INVALID;
}

} // namespace stm