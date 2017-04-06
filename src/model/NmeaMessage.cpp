/**
 * @file NmeaMessage.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include "NmeaMessage.h"

#include <sstream>

namespace stm {

NmeaMessage::NmeaMessage(
	const TalkerId talkerId, const ByteVector & sentenceId,
	const std::vector<ByteVector> & parameters, const uint8_t crc) :
	talkerId(talkerId), sentenceId(sentenceId),
	parameters(parameters.begin(), parameters.end()),
	crc(crc)
{
	updateString();
}

NmeaMessage::NmeaMessage(const NmeaMessage & other) :
	talkerId(other.talkerId), sentenceId(other.sentenceId),
	parameters(other.parameters),
	crc(other.crc), asString(other.asString)
{ }

NmeaMessage::NmeaMessage(NmeaMessage && other) :
	talkerId(other.talkerId), sentenceId(other.sentenceId),
	parameters(std::move(other.parameters)),
	crc(other.crc), asString(std::move(other.asString))
{ }

const std::string & NmeaMessage::toString() const
{
	return asString;
}

const char * NmeaMessage::toCString() const
{
	return asString.c_str();
}

void NmeaMessage::updateString()
{
	std::ostringstream buffer;
	char crcStr[3] = {0};

	snprintf(crcStr, 3, "%02X", crc);

	buffer << '$' << TalkerIdToString(talkerId) << sentenceId;

	for(auto bv : parameters)
	{
		buffer << ',' << bv;
	}

	buffer << '*' << crcStr;

	asString = buffer.str();
}

} // namespace stm