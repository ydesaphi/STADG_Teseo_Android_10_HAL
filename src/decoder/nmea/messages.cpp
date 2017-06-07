/**
 * @file messages.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include "messages.h"

#define LOG_TAG "teseo_hal_nmea_messages"
#include <cutils/log.h>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <mutex>

#include "../../model/Coordinate.h"
#include "../../model/FixQuality.h"
#include "../../model/TalkerId.h"
#include "../../utils/ByteVector.h"
#include "../../utils/Time.h"
#include "../../utils/utils.h"

namespace stm {
namespace decoder {
namespace nmea {

using namespace stm::device;
using namespace stm::utils;

typedef void (*MessageDecoder)(AbstractDevice & dev, const NmeaMessage &);

std::unordered_map<std::string, MessageDecoder> std = {
	{"GGA", &decoders::gga},
	{"VTG", &decoders::vtg},
	{"GSV", &decoders::gsv},
	{"GSA", &decoders::gsa}
};

std::unordered_map<std::string, MessageDecoder> stm = {
	{"SBAS", &decoders::sbas}
};

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
	double HDOP = utils::byteVectorParse<double>(msg.parameters[7]);
	double altitude = utils::byteVectorParse<double>(msg.parameters[8]);

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
}

void decoders::vtg(AbstractDevice & dev, const NmeaMessage & msg)
{
	ALOGI("Decode VTG: %s", msg.toString().c_str());

	double TMGT = utils::byteVectorParse<double>(msg.parameters[0]);
	//double TMGM = utils::byteVectorParseDouble(msg.parameters[2]); // unused
	//double SoGN = utils::byteVectorParseDouble(msg.parameters[4]); // unused
	double SoGK = utils::byteVectorParse<double>(msg.parameters[6]);

	int faaMode = 'A';

	if(msg.parameters.size() > 9)
		faaMode = utils::byteVectorParse<int>(msg.parameters[9]);

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
}

template<typename T>
void gsv_empty_or_set_helper(
	T & out, std::vector<const ByteVector>::iterator & it, bool & emptyValue)
{
	auto value = *it;

	if(value.size() == 0)
	{
		emptyValue = true;
	}
	else
	{
		byteVectorParseAndSet(out, value);
	}

	++it;
}

SatInfo & satellites_get(std::map<SatIdentifier, SatInfo> & sats, const SatIdentifier & id, bool & inserted)
{
	auto insertResult = sats.insert({id, SatInfo(id)});
	inserted = insertResult.second;
	return (*insertResult.first).second;
}

SatInfo & satellites_get(std::map<SatIdentifier, SatInfo> & sats, const SatIdentifier & id)
{
	bool inserted;
	return satellites_get(sats, id, inserted);
}

void decoders::gsv(AbstractDevice & dev, const NmeaMessage & msg)
{
	ALOGI("Decode GSV: %s", msg.toString().c_str());

	// First three parameters are unused
	auto it = msg.parameters.begin() + 3;
	auto & sats = dev.getSatellites();

	//int numberOfSentences  = utils::byteVectorParse<int>(*it); // unused
	//int sentenceId         = utils::byteVectorParse<int>(*it); // unused
	//int NumberOfSatellites = utils::byteVectorParseInt(msg.parameters[2]); // unused
	
	// Extract and update or insert satellites
	while(it != msg.parameters.end())
	{
		bool emptyValue = false, notTracked = false;
		int16_t prn;
		float elevation, azimuth, snr = -1;

		// Extract PRN, Elevation and Azimuth
		// Set empty flag if we found an empty value
		gsv_empty_or_set_helper(prn, it, emptyValue);
		gsv_empty_or_set_helper(elevation, it, emptyValue);
		gsv_empty_or_set_helper(azimuth, it, emptyValue);
		// Extract SNR, if snr is empty notTracked is true
		gsv_empty_or_set_helper(snr, it, notTracked);

		// If any of PRN, Elevation or azimuth was empty we skip this satellite
		if(emptyValue)
		{
			ALOGW("Ignore satellite info with at least one empty value that isn't SNR");
		}
		else
		{
			bool inserted;
			
			SatInfo & s = satellites_get(sats, SatIdentifier(prn), inserted)
				.setElevation(elevation)
				.setAzimuth(azimuth)
				.setSnr(snr)
				.setTracked(!notTracked);

			ALOGI("%s sat prn %d: elevation: %f, azimuth: %f, snr: %f, tracked: %s",
				inserted ? "Insert new" : "Update",
				s.getId().getPrn(),
				elevation,
				azimuth,
				snr,
				!notTracked ? "true" : "false"
			);
		}
	}
}

void decoders::gsa(AbstractDevice & dev, const NmeaMessage & msg)
{
	ALOGI("Decode GSA: %s", msg.toString().c_str());

	// First two parameters are unused
	auto it = msg.parameters.begin() + 2;

	// Selection mode: Auto or Manual - unused
	//char selectionMode = (*it).at(0);

	// Mode: 1 = no fix / 2 = 2D fix / 3 = 3D fix - unused
	//int mode = utils::byteVectorParse<int>(*it);

	auto & sats = dev.getSatellites();

	for(int i = 0; i < 12; i++) {
		if((*it).size() > 0)
		{
			int prn = utils::byteVectorParse<int>(*it);
			satellites_get(sats, SatIdentifier(prn))
				.setUsedInFix(true)
				.setAlmanac(true)
				.setEphemeris(true);
		}

		++it;
	}
}

void decoders::sbas(AbstractDevice & dev, const NmeaMessage & msg)
{
	auto it = msg.parameters.begin();

	bool used = utils::byteVectorParse<bool>(*it);        ++it;
	bool tracked = utils::byteVectorParse<bool>(*it);     ++it;
	int prn = utils::byteVectorParse<int>(*it);           ++it;
	float elevation = utils::byteVectorParse<float>(*it); ++it;
	float azimuth = utils::byteVectorParse<float>(*it);   ++it;
	float snr = utils::byteVectorParse<float>(*it);       ++it;
	bool inserted;

	satellites_get(dev.getSatellites(), SatIdentifier(prn), inserted)
		.setUsedInFix(used)
		.setAlmanac(true)
		.setEphemeris(true)
		.setElevation(elevation)
		.setAzimuth(azimuth)
		.setSnr(snr)
		.setTracked(tracked);

	ALOGI("%s sat prn %d: elevation: %f, azimuth: %f, snr: %f, tracked: %s",
		inserted ? "Insert new" : "Update",
		prn,
		elevation,
		azimuth,
		snr,
		tracked ? "true" : "false");
}

} // namespace nmea
} // namespace decoder
} // namespace stm