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

#include <teseo/utils/Channel.h>

using namespace stm;
using namespace stm::thread;

TEST_CASE( "Channel have correct size", "[thread][Channel]" ) {

	Channel<int> com("unit-test-com");

	REQUIRE(com.size() == 0);

	com << 42;

	REQUIRE(com.size() == 1);

	for(int i = 0; i < 100; i++)
	{
		com << i;
		REQUIRE(com.size() == i + 2);
	}

	int output = -1;

	com >> output;

	REQUIRE(com.size() == 100);
	
	for(int i = 0; i < 50; i++)
	{
		com >> output;
	}

	REQUIRE(com.size() == 50);

	com.clear();

	REQUIRE(com.size() == 0);
}

TEST_CASE( "Channel is receiving and sending in the right order", "[thread][Channel]" ) {
	
		Channel<double> com("unit-test-com");

		com << 42;
		com << 1337;
		com << 3.1415;
		com.send(1.23456);

		REQUIRE(com.receive() == 42);
		REQUIRE(com.receive() == 1337);
		REQUIRE(com.receive() == 3.1415);

		double output;
		com >> output;

		REQUIRE(output == 1.23456);
}