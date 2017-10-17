#include <catch.hpp>

#include <teseo/utils/ByteVector.h>
#include <teseo/utils/Time.h>

using namespace stm;
using namespace stm::utils;

using namespace std;
using namespace std::chrono;

TEST_CASE( "NMEA Time parser works correctly", "[utils][Time]" ) {

	ByteVector time_str = { '1', '3', '3', '7', '4', '2', '.', '0', '0', '0' };

	std::optional<GpsUtcTime> opt_time = parseTimestamp(time_str);

	REQUIRE(static_cast<bool>(opt_time) == true);

	GpsUtcTime time = *opt_time;

	REQUIRE(time == 49062000);

	ByteVector empty = { };

	std::optional<GpsUtcTime> opt_empty = parseTimestamp(empty);

	REQUIRE(static_cast<bool>(opt_empty) == false);
}
