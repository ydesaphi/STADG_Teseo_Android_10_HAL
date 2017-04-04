#include "ByteVector.h"

#define LOG_TAG "teseo_hal_utils_ByteVector"
#include <cutils/log.h>

#include <stdexcept>

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

	for(ByteVector::const_iterator it = start; it != end; ++it)
		out.push_back(*it);

	return out;
}

int byteVectorParseInt(const ByteVector & bytes)
{
	try
	{
		return std::stoi(bytesToString(bytes));
	}
	catch(const std::invalid_argument & ex)
	{
		ALOGE("byteVectorParseInt: Invalid argument: %s '%s'", ex.what(), bytesToString(bytes).c_str());
		return 0;
	}
	catch(...)
	{
		ALOGE("byteVectorParseInt: unknown exeception '%s'", bytesToString(bytes).c_str());
		return 0;
	}
}

int byteVectorParseInt(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
{
	try
	{
		return std::stoi(bytesToString(begin, end));
	}
	catch(const std::invalid_argument & ex)
	{
		ALOGE("byteVectorParseInt: Invalid argument: %s '%s'", ex.what(), bytesToString(begin,end).c_str());
		return 0;
	}
	catch(...)
	{
		ALOGE("byteVectorParseInt: unknown exeception '%s'", bytesToString(begin,end).c_str());
		return 0;
	}
}

double byteVectorParseDouble(const ByteVector & bytes)
{
	try
	{
		return std::stod(bytesToString(bytes));
	}
	catch(const std::invalid_argument & ex)
	{
		ALOGE("byteVectorParseDouble: Invalid argument: %s '%s'", ex.what(), bytesToString(bytes).c_str());
		return 0.;
	}
	catch(...)
	{
		ALOGE("byteVectorParseDouble: unknown exeception '%s'", bytesToString(bytes).c_str());
		return 0;
	}
}

double byteVectorParseDouble(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
{
	try
	{
		return std::stod(bytesToString(begin, end));
	}
	catch(const std::invalid_argument & ex)
	{
		ALOGE("byteVectorParseDouble: Invalid argument: %s '%s'", ex.what(), bytesToString(begin,end).c_str());
		return 0.;
	}
	catch(...)
	{
		ALOGE("byteVectorParseDouble: unknown exeception '%s'", bytesToString(begin,end).c_str());
		return 0;
	}
}

void split(const ByteVector & bytes, uint8_t separator, std::vector<ByteVector> & pieces)
{
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

} // namespace utils
} // namespace stm
