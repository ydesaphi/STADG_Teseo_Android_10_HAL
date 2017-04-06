/**
 * @file messages.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include "messages.h"

#define LOG_TAG "teseo_hal_nmea_messages"
#include <cutils/log.h>
#include <unordered_map>
#include <mutex>

#include "../../model/Coordinate.h"
#include "../../utils/ByteVector.h"
#include "../../utils/Time.h"
#include "../../model/FixQuality.h"

namespace stm {
namespace decoder {
namespace nmea {

using namespace stm::device;
using namespace stm::utils;

typedef void (*MessageDecoder)(AbstractDevice & dev, const NmeaMessage &);

std::unordered_map<std::string, MessageDecoder> std = {
	{"GGA", &decoders::gga},
	{"VTG", &decoders::vtg}
};

std::unordered_map<std::string, MessageDecoder> stm = { };

std::unordered_map<std::string, MessageDecoder> * decoders[] =
{
	&std, // TalkerId::GP
	&std, // TalkerId::GA
	&std, // TalkerId::GL
	&std, // TalkerId::GN
	&std, // TalkerId::GB
	&std, // TalkerId::BD
	&std, // TalkerId::QZ
	&stm  // TalkerId::PSTM
};

MessageDecoder getMessageDecoder(const NmeaMessage & msg)
{
	auto * decodersMap = decoders[static_cast<uint8_t>(msg.talkerId)];

	if(decodersMap == nullptr)
		return nullptr;

	auto sentenceId = bytesToString(msg.sentenceId);

	if(decodersMap->count(sentenceId))
		return (*decodersMap)[sentenceId];

	return nullptr;
}

void decode(AbstractDevice & dev, const NmeaMessage & msg)
{
	MessageDecoder d = getMessageDecoder(msg);

	if(d != nullptr)
		d(dev, msg);
	else
		ALOGW("No decoder for message %s%s", TalkerIdToString(msg.talkerId), bytesToString(msg.sentenceId).c_str());
}

void decoders::gga(AbstractDevice & dev, const NmeaMessage & msg)
{
	ALOGI("Decode GGA: %s", msg.toString().c_str());

	GpsUtcTime timestamp = utils::parseTimestamp(msg.parameters[0]);

	DegreeMinuteCoordinate lat(msg.parameters[1], msg.parameters[2][0]);
	DegreeMinuteCoordinate lon(msg.parameters[3], msg.parameters[4][0]);
	FixQuality quality = FixQualityFromInt(msg.parameters[5][0] - '0');
	double HDOP = utils::byteVectorParseDouble(msg.parameters[7]);
	double altitude = utils::byteVectorParseDouble(msg.parameters[8]);

	dev.setTimestamp(timestamp);

	auto & loc = dev.getLocation();

	if(quality == FixQuality::Invalid)
	{
		loc.invalidateLocation();
		loc.invalidateAltitude();
		loc.invalidateAccuracy();
	}
	else
	{
		loc.latitude(lat.asDecimalDegree().value());
		loc.longitude(lon.asDecimalDegree().value());
		loc.altitude(altitude);
		loc.accuracy(HDOP);
	}

	dev.update();
}

void decoders::vtg(AbstractDevice & dev, const NmeaMessage & msg)
{
	ALOGI("Decode VTG: %s", msg.toString().c_str());

	double TMGT = utils::byteVectorParseDouble(msg.parameters[0]);
	//double TMGM = utils::byteVectorParseDouble(msg.parameters[2]); // unused
	//double SoGN = utils::byteVectorParseDouble(msg.parameters[4]); // unused
	double SoGK = utils::byteVectorParseDouble(msg.parameters[6]);

	int faaMode = 'A';

	if(msg.parameters.size() > 9)
		faaMode = utils::byteVectorParseInt(msg.parameters[9]);

	auto & loc = dev.getLocation();

	if(faaMode != 'N')
	{
		loc.speed(SoGK / 3.6);
		loc.bearing(TMGT);
	}
	else
	{
		loc.invalidateSpeed();
		loc.invalidateBearing();
	}
	
	dev.update();
}

} // namespace nmea
} // namespace decoder
} // namespace stm