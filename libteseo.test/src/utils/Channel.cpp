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