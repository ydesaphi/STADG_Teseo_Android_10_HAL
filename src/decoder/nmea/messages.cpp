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

std::unordered_map<std::string, MessageDecoder> std = {
	{"GGA", &decoders::gga},
	{"VTG", &decoders::vtg}
};

std::unordered_map<std::string, MessageDecoder> * decoders[] =
{
	&std,   // TalkerId::GP
	&std,   // TalkerId::GA
	&std,   // TalkerId::GL
	&std,   // TalkerId::GN
	&std,   // TalkerId::GB
	&std,   // TalkerId::BD
	&std,   // TalkerId::QZ
	nullptr // TalkerId::PSTM
};

void decode(AbstractDevice & dev, const NmeaMessage & msg)
{
	MessageDecoder d = getMessageDecoder(msg);

	if(d != nullptr)
		d(dev, msg);
}

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

	if(quality == FixQuality::Invalid)
	{
		dev.invalidateLocation();
		dev.invalidateAltitude();
		dev.invalidateAccuracy();
	}
	else
	{
		dev.setLocation(lat.asDecimalDegree().value(), lon.asDecimalDegree().value());
		dev.setAltitude(altitude);
		dev.setAccuracy(HDOP * 5);
	}

	dev.update();
}

void decoders::vtg(AbstractDevice & dev, const NmeaMessage & msg)
{
	ALOGI("Decode VTG: %s", msg.toString().c_str());

	double TMGT = utils::byteVectorParseDouble(msg.parameters[0]);
	//double TMGM = utils::byteVectorParseDouble(msg.parameters[2]);
	//double SoGN = utils::byteVectorParseDouble(msg.parameters[4]);
	double SoGK = utils::byteVectorParseDouble(msg.parameters[6]);

	int faaMode = 'A';

	if(msg.parameters.size() > 9)
		faaMode = utils::byteVectorParseInt(msg.parameters[9]);

	if(faaMode != 'N')
	{
		dev.setSpeed(SoGK / 3.6);
		dev.setBearing(TMGT);
	}
	else
	{
		dev.invalidateSpeed();
		dev.invalidateBearing();
	}
	
	dev.update();
}

} // namespace nmea
} // namespace decoder
} // namespace stm