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
 * @brief Version class definition file
 * @file Version.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2017, STMicroelectronics, All rights reserved.
 */
#ifndef TESEO_HAL_MODEL_VERSION_H
#define TESEO_HAL_MODEL_VERSION_H

#include <string>
#include <vector>

namespace stm {
namespace model {

class Version {
public:

	struct Status {
		enum Values : uint8_t {
			STABLE  = 16,
			RC      = 8,
			BETA    = 4,
			ALPHA   = 2,
			TESTING = 1,
			UNKNOWN = 0
		};

		const uint8_t value;

		const uint8_t rcNumber;

		const std::string customStatusString;

		static Status parse(const std::string & statusString);

        bool IsStable() const { return _is(static_cast<uint8_t>(Version::Status::STABLE)); }
        bool IsRC()     const { return _is(static_cast<uint8_t>(Version::Status::RC)); }
        bool IsBeta()   const { return _is(static_cast<uint8_t>(Version::Status::BETA)); }
        bool IsAlpha()  const { return _is(static_cast<uint8_t>(Version::Status::ALPHA)); }
        bool IsTest()   const { return _is(static_cast<uint8_t>(Version::Status::TESTING)); }

		Status();

		Status(Status::Values value);

		Status(uint8_t value, uint8_t rcNumber, const std::string & customStatusString);

		Status(const Status & other);

		Status operator = (Status::Values otherValue);

		Status operator = (const Status & other);

		operator Status::Values () const;

		std::string toString() const;

	private:
		bool _is(uint8_t s) const
		{
			return (value & s) == s;
		}
	};

	enum class Platform : uint8_t {
		UNKNOWN = 0,
		ARM,
		GNU,
		ANY
	};

private:

	/**
	 * Product name
	 */
	std::string product;

	/**
	 * List of version numbers (eg. x.y.z)
	 */
	std::vector<int> versionNumbers;

	/**
	 * Version status (stable, beta...)
	 */
	Status status;

	/**
	 * Product platform
	 */
	Platform platform;

	/**
	 * Build number
	 */
	int buildNumber;

	/**
	 * Specify if version number should be printed as hexadecimal
	 */
	bool versionNumberIsHex;

	/**
	 * String to include before version number
	 */
	std::string includeBeforeVersionNumber;

	std::string buildVersionNumberString() const;

	std::string buildDescriptionString() const;

	void parse(const std::string & str);

	void TokenizeVersionNumber(std::string versionNumberString);

public:

	explicit Version(const std::string & versionString);

	Version();
	
	const std::string & getProduct() const;

	const std::vector<int> & getVersionNumbers() const;

	Status getStatus() const;

	Platform getPlatform() const;

	int getBuildNumber() const;

	std::string toString() const;

	const std::string & platformToString() const;

	static const std::string & platformToString(Platform platform);
};

} // namespace model
} // namespace stm

#endif // TESEO_HAL_MODEL_VERSION_H