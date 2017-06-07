/**
 * @brief Byte vector utilities
 * @file ByteVector.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include "ByteVector.h"

#define LOG_TAG "teseo_hal_utils_ByteVector"
#include <cutils/log.h>

#include <stdexcept>
#include <typeinfo>

namespace stm {
namespace utils {

uint8_t hexCharToValue(uint8_t ch)
{
	bool invalidChar = false;
	return hexCharToValue(ch, invalidChar);
}

uint8_t hexCharToValue(uint8_t ch, bool & invalidChar)
{
	invalidChar = false;

	if(ch >= '0' && ch <= '9')
		return 0xF & (ch - '0');

	if(ch >= 'A' && ch <= 'F')
		return 0xF & (ch - 'A' + 10);

	if(ch >= 'a' && ch <= 'f')
		return 0xF & (ch - 'a' + 10);

	invalidChar = true;
	return 0;
}

uint8_t asciiToByte(uint8_t ch1, uint8_t ch2)
{
	bool invalidChar = false;
	return asciiToByte(ch1, ch2, invalidChar);
}

uint8_t asciiToByte(uint8_t ch1, uint8_t ch2, bool & invalidChar)
{
	bool invalidCh1 = false, invalidCh2 = false;
	uint8_t ret = (hexCharToValue(ch1, invalidCh1) << 4) | hexCharToValue(ch2, invalidCh2);
	invalidChar = invalidCh1 || invalidCh2;
	return 0xFF & ret;
}

std::string bytesToString(const ByteVector & bytes)
{
	std::string out;

	for(auto b : bytes)
		out.push_back(b);

	return out;
}

std::string bytesToString(const ByteVector::const_iterator & start, const ByteVector::const_iterator & end)
{
	std::string out;

	std::for_each(start, end, [&out] (auto val) {
		out.push_back(val);
	});

	return out;
}

std::vector<ByteVector> split(const ByteVector & bytes, uint8_t separator)
{
	std::vector<ByteVector> pieces;
	ByteVector current;

	for(uint8_t b : bytes)
	{
		if(b == separator)
		{
			pieces.push_back(current);
			current.clear();
		}
		else
			current.push_back(b);
	}

	// Push last piece
	pieces.push_back(current);

	// Shrink container and return pieces
	pieces.shrink_to_fit();
	return pieces;
}

ByteVector createFromString(const char * str)
{
	ByteVector vec;

	for(size_t i = 0; i < sizeof(str); i++)
	{
		if(str[i] == '\0')
			break;

		vec.push_back(static_cast<uint8_t>(str[i]));
	}

	return vec;
}

namespace __private {

void __bytevector_parse_log_error(const char * format, const char * what, const char * bytes, int size)
{
	ALOGE(format, what, bytes, size);
}

void __bytevector_parse_log_error(const char * format, const char * bytes, int size)
{
	ALOGE(format, bytes, size);
}

} // namespace private

} // namespace utils
} // namespace stm
