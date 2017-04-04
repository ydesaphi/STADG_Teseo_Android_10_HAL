#ifndef TESEO_HAL_UTILS_BYTE_VECTOR_H
#define TESEO_HAL_UTILS_BYTE_VECTOR_H

#include <vector>
#include <memory>
#include <string>
#include <ostream>

namespace stm {

using ByteVector = std::vector<uint8_t>;

using ByteVectorPtr = std::shared_ptr<ByteVector>;

namespace utils {

uint8_t hexCharToValue(uint8_t ch);

uint8_t hexCharToValue(uint8_t ch, bool & invalidChar);

uint8_t asciiToByte(uint8_t ch1, uint8_t ch2);

uint8_t asciiToByte(uint8_t ch1, uint8_t ch2, bool & invalidChar);

std::string bytesToString(const ByteVector & bytes);

std::string bytesToString(const ByteVector::const_iterator & start, const ByteVector::const_iterator & end);

int byteVectorParseInt(const ByteVector & bytes);

int byteVectorParseInt(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end);

double byteVectorParseDouble(const ByteVector & bytes);

double byteVectorParseDouble(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end);

void split(const ByteVector & bytes, uint8_t separator, std::vector<ByteVector> & pieces);

ByteVector createFromString(const char * str);

} // namespace utils
} // namespace stm

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> & operator << (
	std::basic_ostream<CharT, Traits> & stream, const stm::ByteVector & byteVector)
{
	for(auto b : byteVector)
	{
		stream.put(b);
	}

	return stream;
}

#endif // TESEO_HAL_UTILS_BYTE_VECTOR_H