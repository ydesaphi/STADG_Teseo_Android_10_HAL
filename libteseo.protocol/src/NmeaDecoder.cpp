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
 * @file NmeaDecoder.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/protocol/NmeaDecoder.h>

#define LOG_TAG "teseo_hal_NmeaDecoder"
#include <cutils/log.h>

#include <teseo/model/TalkerId.h>

#include "nmea/messages.h"

#ifdef DEBUG_NMEA_DECODER
#define NMEA_DECODER_LOGE(...) ALOGE(__VA_ARGS__)
#define NMEA_DECODER_LOGI(...) ALOGI(__VA_ARGS__)
#define NMEA_DECODER_LOGW(...) ALOGW(__VA_ARGS__)
#else
#define NMEA_DECODER_LOGE(...)
#define NMEA_DECODER_LOGI(...)
#define NMEA_DECODER_LOGW(...)
#endif

using namespace stm::model;

namespace stm {
namespace decoder {

namespace nmea {

uint8_t extractChecksum(
	const ByteVector & bytes,
	bool & multipleChecksum,
	bool & noChecksum,
	bool & invalidChar)
{
	bool checksumStart = false;
	bool checksumFound = false;
	uint8_t checksumIndex = 0;
	uint8_t checksumBytes[2] = {0};

	multipleChecksum = false;
	noChecksum = false;

	for(uint8_t b : bytes)
	{
		if(checksumStart)
		{
			if(checksumIndex < 2)
			{
				checksumBytes[checksumIndex] = b;
				checksumIndex++;
			}
			else
			{
				checksumFound = true;
				checksumStart = false;
			}
		}

		if(b == '*' && !checksumFound)
		{
			checksumStart = true;
			checksumIndex = 0;
		}
		else if(b == '*')
		{
			NMEA_DECODER_LOGW("Multiple checksum in sentence: '%s'", utils::bytesToString(bytes).c_str());
			multipleChecksum = true;
		}
	}

	if(checksumFound || (checksumStart && checksumIndex == 2))
	{
		return utils::asciiToByte(checksumBytes[0], checksumBytes[1], invalidChar);
	}
	else
	{
		NMEA_DECODER_LOGE("No checksum in sentences.");
		noChecksum = true;
		return 0;
	}
}

uint8_t computeChecksum(const ByteVector & bytes)
{
	bool computeCRC = false;
	uint8_t crc = 0;

	for(uint8_t b: bytes)
	{
		if(b == '*')
		{
			computeCRC = false;
		}

		if(computeCRC)
		{
			crc ^= b;
		}

		if(b == '$')
		{
			computeCRC = true;
		}
	}

	return crc;
}

bool validateChecksum(const ByteVector & bytes, bool & multipleChecksum, uint8_t & crc)
{
	bool noChecksum = false, invalidChar = false;
	uint8_t extractedCRC = extractChecksum(bytes, multipleChecksum, noChecksum, invalidChar);
	uint8_t computedCRC = computeChecksum(bytes);

	crc = computedCRC;

	return extractedCRC == computedCRC && !noChecksum && !invalidChar;
}

} // namespace nmea

NmeaDecoder::NmeaDecoder(device::AbstractDevice & dev) :
	device(dev)
{ }

void NmeaDecoder::decode(ByteVectorPtr bytesPtr)
{
	ByteVector & bytes = *bytesPtr;

	// Message contains at least the following data:
	// $PSTM...*XX
	// So size must be at least more than 8
	if(bytes.size() < 9)
	{
		NMEA_DECODER_LOGE("Sentence is empty or too small to be valid NMEA : '%s'", utils::bytesToString(bytes).c_str());
		return;
	}

	// 1. Validate CRC
	bool multipleChecksum = false;
	uint8_t crc = 0;
	if(!nmea::validateChecksum(bytes, multipleChecksum, crc))
	{
		NMEA_DECODER_LOGE("Invalid checksum in sentence: '%s'", utils::bytesToString(bytes).c_str());
		return;
	}

	// 2. Remove CRC and $
	if(bytes.at(0) == '$')
		bytes.erase(bytes.begin());

	if(multipleChecksum)
	{
		// Find first '*'
		for(auto it = bytes.cbegin(); it != bytes.end(); ++it)
		{
			if(*it == '*')
			{
				bytes.erase(it - 1, bytes.cend());
				break;
			}
		}
	}
	else
	{
		// Find last '*'
		for(auto rit = bytes.crbegin(); rit != bytes.crend(); ++rit)
		{
			if(*rit == '*')
			{
				++rit; // <-- Move the iterator to also remove the star
				// it.base() convert reverse iterator to "normal" iterator
				bytes.erase(rit.base(), bytes.cend());
				break;
			}	
		}
	}


	// 2. Split message
	std::vector<ByteVector> pieces = utils::split(bytes, ',');

	// 3. Extract TalkerID and SentenceID
	const ByteVector & id = pieces[0];

	TalkerId talkerId = ByteVectorToTalkerId(id);
	int talkerIdSize = talkerId == TalkerId::PSTM ? 4 : 2;
	
	ByteVector sentenceId(id.begin() + talkerIdSize, id.end());
	
	// remove message identifier from pieces
	pieces.erase(pieces.begin());

	// 4. Create Message
	NmeaMessage msg(talkerId, sentenceId, pieces, crc);

	// 5. Log NMEA message
	NMEA_DECODER_LOGI("NMEA: '%s'", msg.toCString());

	// 6. Trigger device update before eventually decoding start sequence sentence
	device.updateIfStartSentenceId(msg.sentenceId);

	// 6. Decode message
	nmea::decode(device, msg);

	// 7. Emit NMEA message
	// N.B. Decoding must occur before emit because timestamp may be updated during decode
	device.emitNmea(msg);
}

} // namespace decoder
} // namespace stm