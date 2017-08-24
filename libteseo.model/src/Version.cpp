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
#include <teseo/model/Version.h>

#define LOG_TAG "teseo_hal_Version"
#include <cutils/log.h>

#include <sstream>
#include <unordered_map>
#include <regex>

#include <teseo/utils/utils.h>

namespace stm {
namespace model {

const std::unordered_map<Version::Status::Values, std::string> versionStatusStrings = {
	{Version::Status::STABLE,  "STABLE"},
	{Version::Status::RC,      "RC"},
	{Version::Status::BETA,    "BETA"},
	{Version::Status::ALPHA,   "ALPHA"},
	{Version::Status::TESTING, "TESTING"},
	{Version::Status::UNKNOWN, "UNKNOWN"}
};

const std::unordered_map<Version::Platform, std::string> versionPlatformStrings = {
	{Version::Platform::UNKNOWN, "UNKNOWN"},
	{Version::Platform::ARM,     "ARM"},
	{Version::Platform::GNU,     "GNU"},
	{Version::Platform::ANY,     "ANY"}
};

Version::Status::Status() :
	value(static_cast<uint8_t>(UNKNOWN)),
	rcNumber(0),
	customStatusString("")
{ }

Version::Status::Status(Version::Status::Values value) :
	value(static_cast<uint8_t>(value)),
	rcNumber(0),
	customStatusString("")
{ }

Version::Status::Status(uint8_t value, uint8_t rcNumber, const std::string & customStatusString) :
	value(value),
	rcNumber(rcNumber),
	customStatusString(customStatusString)
{ }

Version::Status::Status(const Version::Status & other) :
	value(other.value),
	rcNumber(other.rcNumber),
	customStatusString(other.customStatusString)
{ }

Version::Status Version::Status::operator = (Version::Status::Values otherValue)
{
	return {static_cast<uint8_t>(otherValue), 0, ""};
}

Version::Status Version::Status::operator = (const Version::Status & other)
{
	return {other.value, other.rcNumber, other.customStatusString};
}

Version::Status Version::Status::parse(const std::string & raw)
{
	static const std::regex rcNumberRe("^.*RC(\\d*).*$");

	std::string::size_type rcPos = raw.find("RC");
	bool beta  = raw.find("BETA") != std::string::npos;
	bool alpha = raw.find("ALPHA") != std::string::npos;
	bool test  = raw.find("TEST") != std::string::npos;
	bool rc    = rcPos != std::string::npos;

	     if ( beta && !alpha && !test && !rc) return {static_cast<uint8_t>(BETA), 0, ""};
	else if (!beta &&  alpha && !test && !rc) return {static_cast<uint8_t>(ALPHA), 0, ""};
	else if (!beta && !alpha &&  test && !rc) return {static_cast<uint8_t>(TESTING), 0, ""};
	else if (!beta && !alpha && !test &&  rc)
	{
		// Get the RC version number or default to 0
		std::string str = std::regex_replace(raw.substr(rcPos), rcNumberRe, "$1");
		uint8_t rcNumber = str.size() > 0 ? std::stoul(str) : 0;

		return {static_cast<uint8_t>(Version::Status::RC), rcNumber, ""};
	}
	else if (beta || alpha || test || rc)
	{
		if (beta && test && !alpha && !rc)
		{
			// Build is specified as BETA TEST
			return {static_cast<uint8_t>(TESTING) | static_cast<uint8_t>(BETA), 0, "BETA_TEST"};
		}
		else if (beta && rc && !alpha && !test)
		{
			// Build is specified as BETA RC

			// Get the RC version number or default to 0
			std::string str = std::regex_replace(raw.substr(rcPos), rcNumberRe, "$1");
			uint8_t rcNumber = str.size() > 0 ? std::stoul(str) : 0;

			std::ostringstream oss;
			oss << "BETA_RC" << rcNumber;
			return {static_cast<uint8_t>(BETA) | static_cast<uint8_t>(RC), rcNumber, oss.str()};
		}
		else
			return {static_cast<uint8_t>(UNKNOWN), 0, ""};
	}
	else
		return {static_cast<uint8_t>(STABLE), 0, ""};
}

std::string Version::Status::toString() const
{
	// Custom string representation
	if (customStatusString.size() > 0)
		return customStatusString;

	// Standard status value
	if(value == static_cast<uint8_t>(STABLE))  return versionStatusStrings.at(STABLE);
	if(value == static_cast<uint8_t>(BETA))    return versionStatusStrings.at(BETA);
	if(value == static_cast<uint8_t>(ALPHA))   return versionStatusStrings.at(ALPHA);
	if(value == static_cast<uint8_t>(TESTING)) return versionStatusStrings.at(TESTING);
	if(value == static_cast<uint8_t>(RC))
	{
		if (rcNumber > 0)
			return (std::ostringstream() << versionStatusStrings.at(RC) << rcNumber).str();
		else
			return versionStatusStrings.at(RC);
	}

	// BETA_RC and BETA_TEST versions use custom status string,
	// we don't need to handle them here.

	// Unknow status
	return versionStatusStrings.at(UNKNOWN);
}

Version::Status::operator Version::Status::Values () const
{
	switch(value)
	{
		case static_cast<uint8_t>(STABLE):  return STABLE;
		case static_cast<uint8_t>(RC):      return RC;
		case static_cast<uint8_t>(BETA):    return BETA;
		case static_cast<uint8_t>(ALPHA):   return ALPHA;
		case static_cast<uint8_t>(TESTING): return TESTING;
		case static_cast<uint8_t>(UNKNOWN): return UNKNOWN;
		default:
			if(IsBeta() && IsRC())
				return RC;

			if(IsBeta() && IsTest())
				return TESTING;
			
			return UNKNOWN;
	}
}

Version::Version() :
	product(""),
	status(Version::Status::STABLE),
	platform(Version::Platform::UNKNOWN),
	buildNumber(0),
	versionNumberIsHex(false)
{ }

Version::Version(const std::string & raw)
{
	parse(raw);
}

void Version::parse(const std::string & raw)
{
	// Split raw
	auto strs = utils::split_if(raw.begin(), raw.end(), [] (const char & ch) {
		return ch == '_' || ch == '-' || ch == '.';
	});

	if (strs.size() == 0)
	{
		product = "";
		status = Version::Status::STABLE;
		platform = Version::Platform::UNKNOWN;
		versionNumberIsHex = false;
		buildNumber = 0;
		return;
	}

	// Extract product name
	product = strs[0];

	if(strs.size() < 2)
	{
		status = Version::Status::STABLE;
		platform = Version::Platform::UNKNOWN;
		versionNumberIsHex = false;
		buildNumber = 0;
		return;
	}

	// Extract version number
	if(strs[1][0] == 'V')
	{
		includeBeforeVersionNumber = "V";
		strs[1].erase(0, 1);
	}

	TokenizeVersionNumber(strs[1]);

	std::string tmp = utils::join(strs.begin() + 2, strs.end(), "_");

	if (tmp.size() == 0)
	{
		status = Version::Status::STABLE;
		buildNumber = 0;
		platform = Version::Platform::UNKNOWN;
		return;
	}

	// Extract version status
	status = Version::Status::parse(tmp);

	// Extract build number
	auto buildPos = tmp.find("BUILD");
	if(buildPos != std::string::npos)
	{
		std::regex buildNumberRe("^.*(BUILD[_ -\\.]?(\\d+)).*$");
		std::string str = std::regex_replace(tmp.substr(buildPos), buildNumberRe, "$2");
		buildNumber = str.size() > 0 ? std::stoi(str) : 0;
	}

	// Extract platform type
	bool containsARM = tmp.find("ARM")    != std::string::npos ||
	                   tmp.find("CP_ARM") != std::string::npos;

	bool containsGNU = tmp.find("GNU") != std::string::npos ||
	                   tmp.find("GCC") != std::string::npos;

	     if ( containsARM && !containsGNU) platform = Version::Platform::ARM;
	else if (!containsARM &&  containsGNU) platform = Version::Platform::GNU;
	else                                   platform = Version::Platform::UNKNOWN;
}

void Version::TokenizeVersionNumber(std::string versionNumberString)
{
	const static std::regex decMatchRe("(.*[^\\d\\.])?(\\d+(\\.\\d+)+).*");
	const static std::regex hexMatchRe("^(.*[^\\da-fA-F])?([\\da-fA-F]+).*$");

	if (std::regex_match(versionNumberString, decMatchRe))
	{
		versionNumberString = std::regex_replace(versionNumberString, decMatchRe, "$2");
		auto strs = utils::split(versionNumberString.begin(), versionNumberString.end(), '.');

		versionNumbers.clear();
		versionNumbers.reserve(strs.size());
		for (std::size_t i = 0; i < strs.size(); i++)
		{
			ALOGI("Version number push: '%s'", strs[i].c_str());
			versionNumbers.push_back(std::stoi(strs[i]));
		}

		versionNumberIsHex = false;
	}
	else if (std::regex_match(versionNumberString, hexMatchRe))
	{
		versionNumberString = std::regex_replace(versionNumberString, hexMatchRe, "$2");
		versionNumbers.clear();
		versionNumbers.reserve(1);
		versionNumbers.push_back(std::stoi(versionNumberString, nullptr, 16));
		versionNumberIsHex = true;
	}
	else
	{
		throw std::runtime_error("String version number is not composed of numbers separated by dots nor an hexadecimal version number.");
	}
}

std::string Version::buildVersionNumberString() const
{
	if(versionNumbers.size() == 0)
		return std::string();

	std::stringstream ss;

	if(includeBeforeVersionNumber.length() > 0)
		ss << includeBeforeVersionNumber;

	if(versionNumberIsHex)
		ss << std::hex;

	auto it = versionNumbers.begin();
	ss << *it;

	for(;it != versionNumbers.end(); ++it)
	{
		ss << "." << *it;
	}

	return ss.str();
}

std::string Version::buildDescriptionString() const
{
	std::stringstream ss;

	if(!status.IsStable())
		ss << "_" << status.toString();

	if(buildNumber > 0)
		ss << "_BUILD_" << buildNumber;

	if(platform == Platform::ARM || platform == Platform::GNU)
		ss << "_" << platformToString();

	return ss.str();
}

const std::string & Version::getProduct() const
{
	return product;
}

const std::vector<int> & Version::getVersionNumbers() const
{
	return versionNumbers;
}

Version::Status Version::getStatus() const
{
	return status;
}

Version::Platform Version::getPlatform() const
{
	return platform;
}

int Version::getBuildNumber() const
{
	return buildNumber;
}

std::string Version::toString() const
{
	return product + "_" + buildVersionNumberString() + buildDescriptionString();
}

const std::string & Version::platformToString() const
{
	return Version::platformToString(platform);
}

const std::string & Version::platformToString(Platform platform)
{
	return versionPlatformStrings.at(platform);
}

} // namespace model
} // namespace stm