/**
 * @brief Byte vector utilities
 * @file ByteVector.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_UTILS_BYTE_VECTOR_H
#define TESEO_HAL_UTILS_BYTE_VECTOR_H

#include <vector>
#include <memory>
#include <string>
#include <ostream>
#include <stdint.h>
#include <memory>

namespace stm {

/**
 * ByteVector type definition
 */
using ByteVector = std::vector<uint8_t>;

/**
 * ByteVector pointer type definition
 */
using ByteVectorPtr = std::shared_ptr<ByteVector>;

namespace utils {

/**
 * @brief      Convert hexadecimal character to its value
 *
 * @param[in]  ch    Character to convert
 *
 * @return     Value of ch, or zero if an error happened
 */
uint8_t hexCharToValue(uint8_t ch);

/**
 * @brief      Convert hexadecimal character to its value
 *
 * @param[in]  ch           Character to convert
 * @param      invalidChar  Boolean set to true if the character is invalid
 *
 * @return     Value of ch, or zero if an error happened
 */
uint8_t hexCharToValue(uint8_t ch, bool & invalidChar);

/**
 * @brief      Convert hexadecimal byte string representation to its value
 *
 * @param[in]  ch1   The first character
 * @param[in]  ch2   The second character
 *
 * @return     The byte value, or zero if an error happened
 */
uint8_t asciiToByte(uint8_t ch1, uint8_t ch2);

/**
 * @brief      Convert hexadecimal byte string representation to its value
 *
 * @param[in]  ch1          The first character
 * @param[in]  ch2          The second character
 * @param      invalidChar  Boolean set to true if the character is invalid
 *
 * @return     The byte value, or zero if an error happened
 */
uint8_t asciiToByte(uint8_t ch1, uint8_t ch2, bool & invalidChar);

/**
 * @brief      Convert an ascii bytes vector to a string
 *
 * @param[in]  bytes  The bytes
 *
 * @return     ASCII bytes as string
 */
std::string bytesToString(const ByteVector & bytes);

/**
 * @brief      Convert an ascii bytes vector to a string
 *
 * @param[in]  start  Byte vector start
 * @param[in]  end    Byte vector end
 *
 * @return     ASCII bytes as string
 */
std::string bytesToString(const ByteVector::const_iterator & start, const ByteVector::const_iterator & end);

template<typename Tout>
struct ByteVectorParser
{
	Tout operator()(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
	{
		(void)(begin); (void)(end);
		static_assert(true, "Missing implementation of ByteVectorParser");
	}

	Tout operator()(const ByteVector & data)
	{
		(void)(data);
		static_assert(true, "Missing implementation of ByteVectorParser");
	}
};

template<>
struct ByteVectorParser<int>
{
	int operator()(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
	{
		if(begin == end)
			return 0;

		return std::stoi(bytesToString(begin, end));
	}

	int operator()(const ByteVector & data)
	{
		if(data.size() == 0)
			return 0;

		return std::stoi(bytesToString(data));
	}
};

template<>
struct ByteVectorParser<double>
{
	double operator()(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
	{
		if(begin == end)
			return 0;
			
		return std::stod(bytesToString(begin, end));
	}

	double operator()(const ByteVector & data)
	{
		if(data.size() == 0)
			return 0;

		return std::stod(bytesToString(data));
	}
};

template<>
struct ByteVectorParser<float>
{
	float operator()(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
	{
		if(begin == end)
			return 0;
			
		return std::stof(bytesToString(begin, end));
	}

	float operator()(const ByteVector & data)
	{
		if(data.size() == 0)
			return 0;

		return std::stof(bytesToString(data));
	}
};

template<>
struct ByteVectorParser<int16_t>
{
	float operator()(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
	{
		if(begin == end)
			return 0;
			
		return static_cast<int16_t>(std::stoi(bytesToString(begin, end)));
	}

	float operator()(const ByteVector & data)
	{
		if(data.size() == 0)
			return 0;
			
		return static_cast<int16_t>(std::stoi(bytesToString(data)));
	}
};

template<>
struct ByteVectorParser<bool>
{
	bool operator()(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
	{
		if(begin == end)
			return false;
			
		return std::stoi(bytesToString(begin, end)) != 0;
	}

	bool operator()(const ByteVector & data)
	{
		if(data.size() == 0)
			return false;
			
		return std::stoi(bytesToString(data)) != 0;
	}
};

namespace __private {
void __bytevector_parse_log_error(const char * format, const char * what, const char * bytes, int size);
void __bytevector_parse_log_error(const char * format, const char * bytes, int size);
} // namespace private

template <typename Tout, class Parser=ByteVectorParser<Tout> >
Tout byteVectorParse(
	const ByteVector::const_iterator & begin,
	const ByteVector::const_iterator & end)
{
	try
	{
		Parser p;
		return p(begin, end);
	}
	catch(const std::invalid_argument & ex)
	{
		__private::__bytevector_parse_log_error(
			"byteVectorParse: Invalid argument: %s, data: '%s' (%d bytes)",
			ex.what(), bytesToString(begin,end).c_str(), end - begin);
		throw;
	}
	catch(const std::exception & ex)
	{
		__private::__bytevector_parse_log_error(
			"byteVectorParse: exception: %s, data: '%s' (%d bytes)",
			ex.what(), bytesToString(begin,end).c_str(), end - begin);
		throw;
	}
	catch(...)
	{
		__private::__bytevector_parse_log_error(
			"byteVectorParse: unknown exception data: '%s' (%d bytes)",
			bytesToString(begin,end).c_str(), end - begin);
		throw;
	}
}

template <typename Tout, class Parser=ByteVectorParser<Tout> >
Tout byteVectorParse(const ByteVector & value)
{
	try
	{
		Parser p;
		return p(value);
	}
	catch(const std::invalid_argument & ex)
	{
		__private::__bytevector_parse_log_error(
			"byteVectorParse: Invalid argument: %s, data: '%s' (%d bytes)",
			ex.what(), bytesToString(value).c_str(), value.size());
		throw;
	}
	catch(const std::exception & ex)
	{
		__private::__bytevector_parse_log_error(
			"byteVectorParse: exception: %s, data: '%s' (%d bytes)",
			ex.what(), bytesToString(value).c_str(), value.size());
		throw;
	}
	catch(...)
	{
		__private::__bytevector_parse_log_error(
			"byteVectorParse: unknown exception, data: '%s' (%d bytes)",
			bytesToString(value).c_str(), value.size());
		throw;
	}
}

template <typename Tout>
void byteVectorParseAndSet(
	Tout & out,
	const ByteVector::const_iterator & begin,
	const ByteVector::const_iterator & end)
{
	out = byteVectorParse<Tout>(begin, end);
}

template <typename Tout>
void byteVectorParseAndSet(Tout & out, const ByteVector & value)
{
	out = byteVectorParse<Tout>(value);
}

/**
 * @brief      Split byte vector at each separator
 *
 * @details    The pieces are appended to pieces, which is not cleared before use
 *
 * @param[in]  bytes      The bytes
 * @param[in]  separator  The separator
 * 
 * @return     The list of pieces
 */
std::vector<ByteVector> split(const ByteVector & bytes, uint8_t separator);

/**
 * @brief      Convert a C-String to a byte vector
 *
 * @param[in]  str   The C-String to convert
 *
 * @return     The converted byte vector
 */
ByteVector createFromString(const char * str);

} // namespace utils
} // namespace stm

/**
 * @brief      Operator overload: ostream << ByteVector
 *
 * @param      stream      The destination stream
 * @param[in]  byteVector  The byte vector to insert in stream
 *
 * @tparam     CharT       Stream character type
 * @tparam     Traits      Stream traits
 *
 * @return     Reference to the stream
 */
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