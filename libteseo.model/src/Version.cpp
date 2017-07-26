#include <teseo/model/Version.h>

#include <sstream>
#include <unordered_map>

namespace stm {
namespace model {

const std::unordered_map<Version::Status, std::string> versionStatusStrings = {
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

std::string Version::buildVersionNumberString() const
{
	if(versionNumbers.size() == 0)
		return std::string();

	std::stringstream ss;

	ss << "_";

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

	if(status != Status::STABLE)
		ss << "_" << statusToString();

	if(buildNumber > 0)
		ss << "_BUILD_" << buildNumber;

	if(platform == Platform::ARM || platform == Platform::GNU)
		ss << "_" << platformToString();

	return ss.str();
}

const std::string & Version::statusToString() const
{
	return Version::statusToString(status);
}

const std::string & Version::platformToString() const
{
	return Version::platformToString(platform);
}

const std::string & Version::statusToString(Status status)
{
	return versionStatusStrings.at(status);
}

const std::string & Version::platformToString(Platform platform)
{
	return versionPlatformStrings.at(platform);
}

} // namespace model
} // namespace stm