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