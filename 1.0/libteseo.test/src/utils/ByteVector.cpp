/*
* This file is part of Teseo Android HAL
*
* Copyright (c) 2016-2018, STMicroelectronics - All Rights Reserved
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

using namespace stm;
using namespace stm::utils;

TEST_CASE( "Byte is converted to ascii", "[utils][ByteVector]" ) {

	REQUIRE( to_ascii(0x42)        == ByteVector({'4', '2'}) );
	REQUIRE( to_ascii(0x42, false) == ByteVector({'4', '2'}) );
	REQUIRE( to_ascii(0x42, true)  == ByteVector({'4', '2'}) );

	REQUIRE( to_ascii(42)        == ByteVector({'2', 'a'}) );
	REQUIRE( to_ascii(42, false) == ByteVector({'2', 'a'}) );
	REQUIRE( to_ascii(42, true)  == ByteVector({'2', 'A'}) );

}

TEST_CASE( "Bytes are converted to ascii", "[utils][ByteVector]" ) {
	
	ByteVector input            = {       0,       15,       16,       42,       19,       55};
	ByteVector output_lowercase = {'0', '0', '0', 'f', '1', '0', '2', 'a', '1', '3', '3', '7'};
	ByteVector output_uppercase = {'0', '0', '0', 'F', '1', '0', '2', 'A', '1', '3', '3', '7'};

	REQUIRE( to_ascii(input)        == output_lowercase );
	REQUIRE( to_ascii(input, false) == output_lowercase );
	REQUIRE( to_ascii(input, true)  == output_uppercase );

}