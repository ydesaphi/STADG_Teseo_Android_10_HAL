/*
* This file is part of Teseo Android HAL
*
* Copyright (c) 2016-2020, STMicroelectronics - All Rights Reserved
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
#include <catch.hpp>

#include <teseo/utils/ByteVector.h>
#include <teseo/utils/Time.h>

using namespace stm;
using namespace stm::utils;

using namespace std;
using namespace std::chrono;

TEST_CASE( "NMEA Time parser works correctly", "[utils][Time]" ) {

	ByteVector time_str = { '1', '3', '3', '7', '4', '2', '.', '0', '0', '0' };

	std::optional<GnssUtcTime> opt_time = parseTimestamp(time_str);

	REQUIRE(static_cast<bool>(opt_time) == true);

	GnssUtcTime time = *opt_time;

	CHECK(time == 49062000);

	/* ByteVector empty = { };

	std::optional<GnssUtcTime> opt_empty = parseTimestamp(empty);

	REQUIRE(static_cast<bool>(opt_empty) == false);
	*/
}
