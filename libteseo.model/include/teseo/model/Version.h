/**
 * @brief Version class definition file
 * @file Version.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2017, STMicroelectronics, All rights reserved.
 */
#ifndef TESEO_HAL_MODEL_VERSION_H
#define TESEO_HAL_MODEL_VERSION_H

#include <string>
#include <list>

namespace stm {
namespace model {

class Version {
public:

	enum class Status : uint8_t {
		STABLE  = 16,
		RC      = 8,
		BETA    = 4,
		ALPHA   = 2,
		TESTING = 1,
		UNKNOWN = 0
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
	std::list<int> versionNumbers;

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

public:

	Version(std::string versionString);

	const std::string & statusToString() const;

	const std::string & platformToString() const;

	static const std::string & statusToString(Status status);

	static const std::string & platformToString(Platform platform);
};

} // namespace model
} // namespace stm

#endif // TESEO_HAL_MODEL_VERSION_H