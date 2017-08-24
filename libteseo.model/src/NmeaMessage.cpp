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
 * @file NmeaMessage.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/model/NmeaMessage.h>
#include <sstream>

#include <teseo/model/TalkerId.h>

namespace stm {

NmeaMessage::NmeaMessage(
	const model::TalkerId talkerId, const ByteVector & sentenceId,
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

	buffer << '$' << model::TalkerIdToString(talkerId) << sentenceId;

	for(auto bv : parameters)
	{
		buffer << ',' << bv;
	}

	buffer << '*' << crcStr;

	asString = buffer.str();
}

} // namespace stm