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

#include <teseo/vendor/frozen/unordered_map.h>
#include <teseo/vendor/frozen/string.h>
#include <teseo/model/Coordinate.h>
#include <teseo/model/FixAndOperatingModes.h>
#include <teseo/model/FixQuality.h>
#include <teseo/model/TalkerId.h>
#include <teseo/utils/ByteVector.h>
#include <teseo/utils/Time.h>
#include <teseo/utils/utils.h>

using namespace frozen::string_literals;

// Messages to debug
#ifndef DISABLE_ALL_MESSAGE_DEBUGGING
	// NMEA std. messages
	//#define MSG_DBG_GGA
	//#define MSG_DBG_VTG
	//#define MSG_DBG_GSV
	//#define MSG_DBG_GSA
	// STMicro proprietary messages
	//#define MSG_DBG_SBAS
	#define MSG_DBG_PSTMVER
	#define MSG_DBG_STAGPS8PASSRTN
	#define MSG_DBG_STAGPSPASSRTN
	#define MSG_DBG_STAGPSSATSEEDRESP
#endif

namespace stm {
namespace decoder {
namespace nmea {

using namespace stm::device;
using namespace stm::utils;
using namespace stm::model;

typedef void (*MessageDecoder)(AbstractDevice & dev, const NmeaMessage &);

constexpr static frozen::unordered_map<frozen::string, MessageDecoder, 4> std = {
	{"GGA"_s, &decoders::gga},
	{"VTG"_s, &decoders::vtg},
	{"GSV"_s, &decoders::gsv},
	{"GSA"_s, &decoders::gsa}
	// Do not forget to update number of elements in map declaration
};

constexpr static frozen::unordered_map<frozen::string, MessageDecoder, 8> stm = {
	{"SBAS"_s, &decoders::sbas},
	{"VER"_s,  &decoders::pstmver},
	{"STAGPS8PASSRTN"_s,  &decoders::pstmstagps8passrtn},
	{"STAGPS8PASSGENERROR"_s, &decoders::pstmstagps8passrtn},
	{"STAGPSPASSRTN"_s,  &decoders::pstmstagpspassrtn},
	{"STAGPSPASSGENERROR"_s, &decoders::pstmstagpspassrtn},
	{"STAGPSSATSEEDOK"_s, &decoders::pstmstagpssatseedresponse},
	{"STAGPSSATSEEDERROR"_s, &decoders::pstmstagpssatseedresponse}
	// Do not forget to update number of elements in map declaration
};

MessageDecoder getMessageDecoder(const NmeaMessage & msg)
{
	frozen::string sid(bytesToString(msg.sentenceId));

	if(msg.talkerId == TalkerId::PSTM)
	{
		auto it = stm.find(sid);

		#ifdef DEBUG_NMEA_DECODER
		if(it == stm.end())
		{
			ALOGW("No decoder for proprietary message, talkerId: '%s', sentenceId: '%s'",
				TalkerIdToString(msg.talkerId),
				utils::bytesToString(msg.sentenceId).c_str());
		}
		#endif

		return (it != stm.end()) ? (*it).second : nullptr;
	}
	else
	{
		auto it = std.find(sid);

		#ifdef DEBUG_NMEA_DECODER
		if(it == std.end())
		{
			ALOGW("No decoder for standard message, talkerId: '%s', sentenceId: '%s'",
				TalkerIdToString(msg.talkerId),
				utils::bytesToString(msg.sentenceId).c_str());
		}
		#endif

		return (it != std.end()) ? (*it).second : nullptr;
	}
}

void decode(AbstractDevice & dev, const NmeaMessage & msg)
{
	MessageDecoder d = getMessageDecoder(msg);

	if(d != nullptr)
		d(dev, msg);
#ifdef DEBUG_NMEA_DECODER
	else
	{
		ALOGW("Decoder is nullptr.");
	}
#endif
}

#ifdef MSG_DBG_GGA
#define GGA_LOGI(...) ALOGI(__VA_ARGS__)
#define GGA_LOGW(...) ALOGW(__VA_ARGS__)
#else
#define GGA_LOGI(...)
#define GGA_LOGW(...)
#endif
void decoders::gga(AbstractDevice & dev, const NmeaMessage & msg)
{
	GGA_LOGI("Decode GGA: %s", msg.toString().c_str());

	GpsUtcTime timestamp = 0;
	
	if(auto opt = utils::parseTimestamp(msg.parameters[0]))
		timestamp = *opt;
	else
	{
		timestamp = utils::systemNow();
		GGA_LOGW("Error while parsing GGA timestamp, defaulted to system now.");
	}

	DegreeMinuteCoordinate lat(msg.parameters[1], msg.parameters[2][0]);
	DegreeMinuteCoordinate lon(msg.parameters[3], msg.parameters[4][0]);
	FixQuality quality = FixQualityFromInt(msg.parameters[5][0] - '0');
	double HDOP = utils::byteVectorParse<double>(msg.parameters[7]).value_or(0);
	double altitude = utils::byteVectorParse<double>(msg.parameters[8]).value_or(0);

	dev.setTimestamp(timestamp);

	auto locResult = dev.getLocation();
	Location loc = locResult ? *locResult : Location();

	loc.quality(quality);
	loc.timestamp(timestamp);

	if(quality == FixQuality::Invalid)
	{
		loc.invalidateLocation();
		loc.invalidateAltitude();
		loc.invalidateAccuracy();
	}
	else
	{
		loc.location(lat.asDecimalDegree().value(),
		             lon.asDecimalDegree().value());
		loc.altitude(altitude);
		loc.accuracy(HDOP);
	}

	dev.setLocation(loc);
}

#ifdef MSG_DBG_VTG
#define VTG_LOGI(...) ALOGI(__VA_ARGS__)
#define VTG_LOGW(...) ALOGW(__VA_ARGS__)
#else
#define VTG_LOGI(...)
#define VTG_LOGW(...)
#endif
void decoders::vtg(AbstractDevice & dev, const NmeaMessage & msg)
{
	VTG_LOGI("Decode VTG: %s", msg.toString().c_str());

	double TMGT = utils::byteVectorParse<double>(msg.parameters[0]).value_or(0.);
	//double TMGM = utils::byteVectorParseDouble(msg.parameters[2]); // unused
	//double SoGN = utils::byteVectorParseDouble(msg.parameters[4]); // unused
	double SoGK = utils::byteVectorParse<double>(msg.parameters[6]).value_or(0.);

	int faaMode = 'A';

	if(msg.parameters.size() > 9)
		faaMode = utils::byteVectorParse<int>(msg.parameters[9]).value_or(0);

	auto locResult = dev.getLocation();
	Location loc = locResult ? *locResult : Location();

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

	dev.setLocation(loc);
}

#ifdef MSG_DBG_GSV
#define GSV_LOGI(...) ALOGI(__VA_ARGS__)
#define GSV_LOGW(...) ALOGW(__VA_ARGS__)
#define GSV_LOGE(...) ALOGE(__VA_ARGS__)
#else
#define GSV_LOGI(...)
#define GSV_LOGW(...)
#define GSV_LOGE(...)
#endif
template<typename T>
void gsv_empty_or_set_helper(
	T & out,
	std::vector<const ByteVector>::iterator & it,
	std::vector<const ByteVector>::iterator end,
	bool & emptyValue, T defaultValue)
{
	if(it >= end)
	{
		emptyValue = true;
		return;
	}

	auto value = *it;

	if(value.size() == 0)
	{
		emptyValue = true;
	}
	else
	{
		out = byteVectorParse<T>(value).value_or(defaultValue);
	}

	++it;
}

template<typename T>
bool gsv_empty_or_set_helper(
	T & out,
	std::vector<const ByteVector>::iterator & it,
	std::vector<const ByteVector>::iterator end,
	bool & emptyValue)
{
	if(it >= end)
	{
		emptyValue = true;
		return false;
	}

	auto value = *it;
	bool ret = true;

	if(value.size() == 0)
	{
		emptyValue = true;
	}
	else if(auto opt = byteVectorParse<T>(value))
	{
		out = *opt;
	}
	else
	{
		ret = false;
	}

	++it;
	
	return ret;
}

void decoders::gsv(AbstractDevice & dev, const NmeaMessage & msg)
{
	GSV_LOGI("Decode GSV: %s", msg.toString().c_str());

	// First three parameters are unused
	auto it = msg.parameters.begin() + 3;

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
		if(!gsv_empty_or_set_helper(prn, it, msg.parameters.end(), emptyValue))
		{
			GSV_LOGE("Unable to parse PRN from message: '%s'", msg.toCString());
			return; // Error during value parse
		}

		gsv_empty_or_set_helper(elevation, it, msg.parameters.end(), emptyValue, static_cast<float>(0.));
		gsv_empty_or_set_helper(azimuth, it, msg.parameters.end(), emptyValue, static_cast<float>(0.));
		// Extract SNR, if snr is empty notTracked is set to true
		gsv_empty_or_set_helper(snr, it, msg.parameters.end(), notTracked, static_cast<float>(0.));

		// If any of PRN, Elevation or azimuth was empty we skip this satellite
		if(emptyValue)
		{
			GSV_LOGW("Ignore satellite info with at least one empty value that isn't SNR");
		}
		else
		{
			SatIdentifier id(prn);

			auto result = dev.getSatellite(id);
			SatInfo s = result ? *result : SatInfo(id);
			
			s.setElevation(elevation)
			 .setAzimuth(azimuth)
			 .setSnr(snr)
			 .setTracked(!notTracked);

			dev.addSatellite(s);

			GSV_LOGI("%s sat prn %d: elevation: %f, azimuth: %f, snr: %f, tracked: %s",
				result ? "Update" : "Insert new",
				s.getId().getPrn(),
				elevation,
				azimuth,
				snr,
				!notTracked ? "true" : "false"
			);
		}
	}
}

#ifdef MSG_DBG_GSA
#define GSA_LOGI(...) ALOGI(__VA_ARGS__)
#define GSA_LOGW(...) ALOGW(__VA_ARGS__)
#define GSA_LOGE(...) ALOGE(__VA_ARGS__)
#else
#define GSA_LOGI(...)
#define GSA_LOGW(...)
#define GSA_LOGE(...)
#endif
void decoders::gsa(AbstractDevice & dev, const NmeaMessage & msg)
{
	GSA_LOGI("Decode GSA: %s", msg.toString().c_str());

	// First two parameters are unused
	auto it = msg.parameters.begin() + 1;

	// Selection mode: Auto or Manual - unused
	//char selectionMode = (*it).at(0);

	// Mode: 1 = no fix / 2 = 2D fix / 3 = 3D fix - unused
	FixMode mode = FixModeFromChar((*it).at(0));
	++it;

	// Update fix mode
	auto locResult = dev.getLocation();
	Location loc = locResult ? *locResult : Location();
	loc.fixMode(mode);
	dev.setLocation(loc);

	// Update satellites usage informations
	for(int i = 0; i < 12; i++)
	{
		if((*it).size() > 0)
		{
			if(auto opt = utils::byteVectorParse<int>(*it))
			{
				SatIdentifier id(*opt);
				
				auto result = dev.getSatellite(id);
				SatInfo s = result ? *result : SatInfo(id);
				
				s.setUsedInFix(true)
				.setAlmanac(true)
				.setEphemeris(true);

				dev.addSatellite(s);
			}
		}

		++it;
	}
}

#ifdef MSG_DBG_SBAS
#define SBAS_LOGI(...) ALOGI(__VA_ARGS__)
#define SBAS_LOGW(...) ALOGW(__VA_ARGS__)
#define SBAS_LOGE(...) ALOGE(__VA_ARGS__)
#else
#define SBAS_LOGI(...)
#define SBAS_LOGW(...)
#define SBAS_LOGE(...)
#endif
void decoders::sbas(AbstractDevice & dev, const NmeaMessage & msg)
{
	SBAS_LOGI("Decode SBAS: %s", msg.toString().c_str());
	auto it = msg.parameters.begin();

	bool used    = utils::byteVectorParse<bool>(*it).value_or(true); ++it;
	bool tracked = utils::byteVectorParse<bool>(*it).value_or(true); ++it;

	SatIdentifier id;
	if(auto opt = utils::byteVectorParse<int>(*it))
		id = SatIdentifier(*opt);
	else
		return;
	
	++it;

	float elevation = utils::byteVectorParse<float>(*it).value_or(0.); ++it;
	float azimuth   = utils::byteVectorParse<float>(*it).value_or(0.); ++it;
	float snr       = utils::byteVectorParse<float>(*it).value_or(0.); ++it;

	auto result = dev.getSatellite(id);
	SatInfo s = result ? *result : SatInfo(id);

	s.setUsedInFix(used)
	 .setAlmanac(true)
	 .setEphemeris(true)
	 .setElevation(elevation)
	 .setAzimuth(azimuth)
	 .setSnr(snr)
	 .setTracked(tracked);

	dev.addSatellite(s);

	SBAS_LOGI("%s sat prn %d: elevation: %f, azimuth: %f, snr: %f, tracked: %s",
		result ? "Update" : "Insert new",
		id.getPrn(),
		elevation,
		azimuth,
		snr,
		tracked ? "true" : "false");
}

#ifdef MSG_DBG_PSTMVER
#define PSTMVER_LOGI(...) ALOGI(__VA_ARGS__)
#define PSTMVER_LOGW(...) ALOGW(__VA_ARGS__)
#define PSTMVER_LOGE(...) ALOGE(__VA_ARGS__)
#else
#define PSTMVER_LOGI(...)
#define PSTMVER_LOGW(...)
#define PSTMVER_LOGE(...)
#endif
void decoders::pstmver(AbstractDevice & dev, const NmeaMessage & msg)
try
{
	PSTMVER_LOGI("Decode PSTMVER: %s", msg.toCString());

	model::Version v(bytesToString(msg.parameters.at(0)));

	PSTMVER_LOGI("Product: %s, version string: %s", v.getProduct().c_str(), v.toString().c_str());

	dev.newVersionNumber(v);
}
catch(const std::exception & ex)
{
	ALOGE("Error while parsing version number: %s", ex.what());
}
catch(...)
{
	ALOGE("Error while parsing version number.");
}

#ifdef MSG_DBG_STAGPS8PASSRTN
#define STAGPS8PASSRTN_LOGI(...) ALOGI(__VA_ARGS__)
#define STAGPS8PASSRTN_LOGW(...) ALOGW(__VA_ARGS__)
#define STAGPS8PASSRTN_LOGE(...) ALOGE(__VA_ARGS__)
#else
#define STAGPS8PASSRTN_LOGI(...)
#define STAGPS8PASSRTN_LOGW(...)
#define STAGPS8PASSRTN_LOGE(...)
#endif
void decoders::pstmstagps8passrtn(AbstractDevice & dev, const NmeaMessage & msg)
{
	if(msg.sentenceId == utils::createFromString("STAGPS8PASSGENERROR"))
	{
		STAGPS8PASSRTN_LOGI("Decode PSTMSTAGPS8PASSGENERROR");
		dev.onStagps8Answer(model::Stagps8Answer::PasswordReturnKO, { });
	}
	else
	{
		STAGPS8PASSRTN_LOGI("Decode PSTMSTAGPS8PASSRTN: %s", msg.toCString());
		STAGPS8PASSRTN_LOGI("Password string: %s", bytesToString(msg.parameters.at(0)).c_str());
		dev.onStagps8Answer(model::Stagps8Answer::PasswordReturnOk, { msg.parameters.at(0) });
	}
}


#ifdef MSG_DBG_STAGPSPASSRTN
#define STAGPSPASSRTN_LOGI(...) ALOGI(__VA_ARGS__)
#define STAGPSPASSRTN_LOGW(...) ALOGW(__VA_ARGS__)
#define STAGPSPASSRTN_LOGE(...) ALOGE(__VA_ARGS__)
#else
#define STAGPSPASSRTN_LOGI(...)
#define STAGPSPASSRTN_LOGW(...)
#define STAGPSPASSRTN_LOGE(...)
#endif
void decoders::pstmstagpspassrtn(AbstractDevice & dev, const NmeaMessage & msg)
{
	if(msg.sentenceId == utils::createFromString("STAGPSPASSGENERROR"))
	{
		STAGPSPASSRTN_LOGI("Decode PSTMSTAGPSPASSGENERROR");
		dev.onStagpsAnswer(model::StagpsAnswer::PasswordReturnKO, { });
	}
	else
	{
		STAGPSPASSRTN_LOGI("Decode PSTMSTAGPSPASSRTN: %s", msg.toCString());
		STAGPSPASSRTN_LOGI("Password string: %s", bytesToString(msg.parameters.at(0)).c_str());
		dev.onStagpsAnswer(model::StagpsAnswer::PasswordReturnOk, { msg.parameters.at(0) });
	}
}


#ifdef MSG_DBG_STAGPSSATSEEDRESP
#define STAGPSSATSEEDRESP_LOGI(...) ALOGI(__VA_ARGS__)
#define STAGPSSATSEEDRESP_LOGW(...) ALOGW(__VA_ARGS__)
#define STAGPSSATSEEDRESP_LOGE(...) ALOGE(__VA_ARGS__)
#else
#define STAGPSSATSEEDRESP_LOGI(...)
#define STAGPSSATSEEDRESP_LOGW(...)
#define STAGPSSATSEEDRESP_LOGE(...)
#endif
void decoders::pstmstagpssatseedresponse(AbstractDevice &, const NmeaMessage & msg)
{
	if(msg.sentenceId == utils::createFromString("STAGPSSATSEEDERROR"))
	{
		STAGPSSATSEEDRESP_LOGI("Device sent STAGPSSATSEEDERROR");
	}
	if(msg.sentenceId == utils::createFromString("STAGPSSATSEEDOK"))
	{
		STAGPSSATSEEDRESP_LOGI("Device sent STAGPSSATSEEDOK");
	}
}

} // namespace nmea
} // namespace decoder
} // namespace stm