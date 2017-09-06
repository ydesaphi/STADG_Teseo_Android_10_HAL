/*
* This file is part of Teseo Android HAL
*
* Copyright (c) 2016-2017, STMicroelectronics - All Rights Reserved
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
#include <cstdint>
#include <memory>
#include <array>

#include "optional.h"

#include <hardware/gps.h>

namespace stm {

/**
 * ByteVector type definition
 */
using ByteVector = std::vector<uint8_t>;

/**
 * ByteArray type definition
 */
template<std::size_t N>
using ByteArray = std::array<uint8_t, N>;

/**
 * ByteVector pointer type definition
 */
using ByteVectorPtr = std::shared_ptr<ByteVector>;

/**
 * ByteArray pointer type definition
 */
template<std::size_t N>
using ByteArrayPtr = std::shared_ptr<ByteArray<N>>;

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
 *
 * @details The following bytes : `{102, 111, 111}` will be converted to the following
 * string: "foo", or `{'b', 'a', 'r'}` to "bar".
 */
std::string bytesToString(const ByteVector & bytes);

/**
 * @brief      Convert an ascii bytes array to a string
 *
 * @param[in]  bytes  The bytes
 *
 * @return     ASCII bytes as string
 *
 * @details The following bytes : ``{102, 111, 111}` will be converted to the following
 * string: "foo", or `{'b', 'a', 'r'}` to "bar".
 */
template<std::size_t N>
std::string bytesToString(const ByteArray<N> & bytes)
{
	std::string out;

	for(auto b : bytes)
		out.push_back(b);

	return out;
}

/**
 * @brief      Convert an ascii bytes vector to a string
 *
 * @param[in]  start  Byte vector start
 * @param[in]  end    Byte vector end
 *
 * @return     ASCII bytes as string
 *
 * @details The following bytes : ``{102, 111, 111}` will be converted to the following
 * string: "foo", or `{'b', 'a', 'r'}` to "bar".
 */
std::string bytesToString(
	const ByteVector::const_iterator & start,
	const ByteVector::const_iterator & end);

/**
 * @brief Template for byte vector parser
 *
 * @tparam Tout Parser output type
 *
 * @details A ByteVectorParser will parse values from ByteVector containing ASCII characters.
 * To add another parser you need to specialize the template. See ByteVectorParser<int> for example.
 */
template<typename Tout>
struct ByteVectorParser
{
	/**
	 * Parse a byte vector
	 *
	 * @param[in]  begin Byte vector begin
	 * @param[in]  end   Byte vector end
	 *
	 * @return     The parsed value, or an empty value.
	 */
	std::optional<Tout> operator()(
		const ByteVector::const_iterator & begin,
		const ByteVector::const_iterator & end)
	{
		(void)(begin); (void)(end);
		static_assert(true, "Missing implementation of ByteVectorParser");
		return {};
	}

	/**
	 * Parse a byte vector
	 *
	 * @param[in]  data The byte vector
	 *
	 * @return     The parsed value, or an empty value.
	 */
	std::optional<Tout> operator()(const ByteVector & data)
	{
		(void)(data);
		static_assert(true, "Missing implementation of ByteVectorParser");
		return {};		
	}
};

/**
 * @brief Byte vector integer parser.
 *
 * @todo Avoid string convertion, directly parse the byte vector.
 */
template<>
struct ByteVectorParser<int>
{
	std::optional<int> operator()(
		const ByteVector::const_iterator & begin,
		const ByteVector::const_iterator & end)
	{
		if(begin == end)
			return {};

		return std::stoi(bytesToString(begin, end));
	}

	std::optional<int> operator()(const ByteVector & data)
	{
		if(data.size() == 0)
			return {};

		return std::stoi(bytesToString(data));
	}
};

/**
 * @brief Byte vector double precision number parser.
 *
 * @todo Avoid string convertion, directly parse the byte vector.
 */
template<>
struct ByteVectorParser<double>
{
	std::optional<double> operator()(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
	{
		if(begin == end)
			return {};
			
		return std::stod(bytesToString(begin, end));
	}

	std::optional<double> operator()(const ByteVector & data)
	{
		if(data.size() == 0)
			return {};

		return std::stod(bytesToString(data));
	}
};

/**
 * @brief Byte vector single precision number parser.
 *
 * @todo Avoid string convertion, directly parse the byte vector.
 */
template<>
struct ByteVectorParser<float>
{
	std::optional<float> operator()(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
	{
		if(begin == end)
			return {};
			
		return std::stof(bytesToString(begin, end));
	}

	std::optional<float> operator()(const ByteVector & data)
	{
		if(data.size() == 0)
			return {};

		return std::stof(bytesToString(data));
	}
};

/**
 * @brief Byte vector short integer (16 bits, 2 bytes) parser.
 *
 * @todo Avoid string convertion, directly parse the byte vector.
 */
template<>
struct ByteVectorParser<int16_t>
{
	std::optional<int16_t> operator()(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
	{
		if(begin == end)
			return {};
			
		return static_cast<int16_t>(std::stoi(bytesToString(begin, end)));
	}

	std::optional<int16_t> operator()(const ByteVector & data)
	{
		if(data.size() == 0)
			return {};
			
		return static_cast<int16_t>(std::stoi(bytesToString(data)));
	}
};

/**
 * @brief Byte vector boolean parser.
 *
 * @details Parse numeric boolean, 0 is false, all other values are true.
 *
 * @todo Avoid string convertion, directly parse the byte vector.
 */
template<>
struct ByteVectorParser<bool>
{
	std::optional<bool> operator()(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
	{
		if(begin == end)
			return {};
			
		return std::stoi(bytesToString(begin, end)) != 0;
	}

	std::optional<bool> operator()(const ByteVector & data)
	{
		if(data.size() == 0)
			return {};
			
		return std::stoi(bytesToString(data)) != 0;
	}
};

namespace __private {
void __bytevector_parse_log_error(const char * format, const char * what, const char * bytes, int size);
void __bytevector_parse_log_error(const char * format, const char * bytes, int size);
} // namespace private

/**
 * @brief Generic byte vector parser function
 *
 * @param begin   Iterator to the begining of the byte vector to parse
 * @param end     Iterator to the end of the byte vector to parse
 * 
 * @tparam Tout   The type of value to parse
 * @tparam Parser The parser to use, automatically deduced from Tout
 *
 * @return        The parsed value, or empty value.
 */
template <typename Tout, class Parser=ByteVectorParser<Tout> >
std::optional<Tout> byteVectorParse(
	const ByteVector::const_iterator & begin,
	const ByteVector::const_iterator & end) noexcept
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
		return {};
	}
	catch(const std::exception & ex)
	{
		__private::__bytevector_parse_log_error(
			"byteVectorParse: exception: %s, data: '%s' (%d bytes)",
			ex.what(), bytesToString(begin,end).c_str(), end - begin);
		return {};
	}
	catch(...)
	{
		__private::__bytevector_parse_log_error(
			"byteVectorParse: unknown exception data: '%s' (%d bytes)",
			bytesToString(begin,end).c_str(), end - begin);
		return {};
	}
}

/**
 * @brief Generic byte vector parser function
 *
 * @param value   The byte vector to parse
 * 
 * @tparam Tout   The type of value to parse
 * @tparam Parser The parser to use, automatically deduced from Tout
 *
 * @return        The parsed value, or empty value.
 */
template <typename Tout, class Parser=ByteVectorParser<Tout> >
std::optional<Tout> byteVectorParse(const ByteVector & value) noexcept
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
		return {};
	}
	catch(const std::exception & ex)
	{
		__private::__bytevector_parse_log_error(
			"byteVectorParse: exception: %s, data: '%s' (%d bytes)",
			ex.what(), bytesToString(value).c_str(), value.size());
		return {};
	}
	catch(...)
	{
		__private::__bytevector_parse_log_error(
			"byteVectorParse: unknown exception, data: '%s' (%d bytes)",
			bytesToString(value).c_str(), value.size());
		return {};
	}
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
 *
 * @details    This function outputs ASCII encoded string
 */
ByteVector createFromString(const char * str);

/**
 * @brief      Convert a std::string to a byte vector
 *
 * @param[in]  str   The std::string to convert
 *
 * @return     The converted byte vector
 *
 * @details    This function outputs ASCII encoded string
 */
ByteVector createFromString(const std::string & str);

namespace impl {

	template<std::size_t N, std::size_t... I>
	constexpr ByteArray<N> BytestoArray(uint8_t (&a)[N], std::index_sequence<I...>)
	{
		return { {a[I]...} };
	}

	template<std::size_t N, std::size_t... I>
	constexpr ByteArray<N> str2ba(const char * data, std::index_sequence<I...>)
	{
		return { {(static_cast<uint8_t>(data[I]))...} };
	}

	constexpr ByteArray<2> to_ascii(uint8_t byte, bool uppercase);

	template<std::size_t N>
	constexpr ByteArray<N> join_array(
		ByteArray<N> & output,
		std::size_t & pos,
		const ByteArray<2> array)
	{
		if(pos > output.size())
			return output;

		output[pos++] = array[0];
		output[pos++] = array[1];

		return output;
	}

	template<std::size_t N, typename ...Rest>
	constexpr ByteArray<N> join_array(
		ByteArray<N> & output,
		std::size_t & pos,
		const ByteArray<2> array,
		Rest... arrays)
	{
		if(pos > output.size())
			return output;

		output[pos++] = array[0];
		output[pos++] = array[1];

		return join_array(output, pos, arrays...);
	}

	template<std::size_t N, typename ...Rest>
	constexpr ByteArray<N> join_arrays(const ByteArray<2> & array, Rest... arrays)
	{
		ByteArray<N> output;
		std::size_t pos = 2;

		output[0] = array[0];
		output[1] = array[1];

		return join_array(output, pos, arrays...);
	}

	template<std::size_t N, std::size_t... I>
	constexpr ByteArray<2*N> to_ascii(const ByteArray<N> bytes, bool uppercase, std::index_sequence<I...>)
	{
		return join_arrays<2*N>((to_ascii(bytes.at(I), uppercase))...);
	}

} // namespace impl

template<std::size_t N>
constexpr ByteArray<N> str2ba(const char * data)
{
	return impl::str2ba<N>(data, std::make_index_sequence<N>{});
}

template<std::size_t N>
constexpr ByteArray<N> BytestoArray(uint8_t (&a)[N])
{
	return impl::BytestoArray(a, std::make_index_sequence<N>{});
}

template<typename T>
typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, T>::type
extract_from_bv(ByteVector::const_iterator begin, ByteVector::const_iterator end)
{
	std::size_t nbBytes = sizeof(T);
	T value = 0;
	int shift = (nbBytes - 1) * 8;

	for(std::size_t i = 0; i < nbBytes && begin != end; i++)
	{
		value |= (*begin << shift);
		shift -= 8;
		++begin;
	}

	return value;
}

template<uint8_t>
uint8_t extract_from_bv(ByteVector::const_iterator begin, ByteVector::const_iterator end)
{
	return begin == end ? 0 : *begin;
}

template<uint16_t>
uint16_t extract_from_bv(ByteVector::const_iterator begin, ByteVector::const_iterator end)
{
	ByteVector::const_iterator b1, b2;
	switch(end - begin)
	{
		case 0: return 0;
		case 1: return ((*begin) << 8) & 0xFF00;
		default:
			b1 = begin; ++begin;
			b2 = begin;
			return 0xFFFF & ((*b1 << 8) & 0xFF00) &
			                ((*b2     ) & 0x00FF);
	}
}

template<uint32_t>
uint32_t extract_from_bv(ByteVector::const_iterator begin, ByteVector::const_iterator end)
{
	ByteVector::const_iterator b1, b2, b3, b4;

	switch(end - begin)
	{
		case 0: return 0;
		case 1: return ((*begin) << 24) & 0xFF00;
		case 2:
			b1 = begin; ++begin;
			b2 = begin;
			return 0xFFFF0000 & ((*b1 << 24) & 0xFF000000) &
			                    ((*b2 << 16) & 0x00FF0000);

		case 3:
			b1 = begin; ++begin;
			b2 = begin; ++begin;
			b3 = begin;
			return 0xFFFFFF00 & ((*b1 << 24) & 0xFF000000) &
			                    ((*b2 << 16) & 0x00FF0000) &
			                    ((*b3 <<  8) & 0x0000FF00);

		default:
			b1 = begin; ++begin;
			b2 = begin; ++begin;
			b3 = begin; ++begin;
			b4 = begin;
			return 0xFFFFFFFF & ((*b1 << 24) & 0xFF000000) &
				                ((*b2 << 16) & 0x00FF0000) &
				                ((*b3 <<  8) & 0x0000FF00) &
				                ((*b4      ) & 0x000000FF);
	}
}

/**
 * Decodes a string containing base64 encoded data
 *
 * @param base64 Base 64 encoded data
 *
 * @return Decoded bytes
 */
ByteVector base64_decode(const std::string & base64);

/**
 * Encodes bytes to base 64 data
 *
 * @param bytes Bytes to encode
 *
 * @return Encoded bytes as a string
 */
std::string base64_encode(const ByteVector & bytes);

/**
 * @brief Convert bytes to their hexadecimal values in ASCII.
 *
 * @param bytes      Byte array to convert
 * @param uppercase  Convert to uppercase hexadecimal, or not
 *
 * @tparam N         Size of the array
 *
 * @return A ByteArray containing the hexadecimal bytes values in ASCII.
 *
 * @details This function convert the following bytes `{0, 15, 16, 42, 19, 55}` to the following
 * bytes: `{0x30, 0x30, 0x30, 0x66, 0x31, 0x30, 0x32, 0x61, 0x31, 0x33, 0x33, 0x37} alternatively
 * said: `{'0', '0', '0', 'f', '1', '0', '2', 'a', '1', '3', '3', '7'}`.
 */
template<std::size_t N>
constexpr ByteArray<2*N> to_ascii(const ByteArray<N> & bytes, bool uppercase = false)
{
	return impl::to_ascii<N>(bytes, uppercase, std::make_index_sequence<N>{});
}

/**
 * @brief Convert bytes to their hexadecimal values in ASCII.
 *
 * @param bytes      Byte vector to convert
 * @param uppercase  Convert to uppercase hexadecimal, or not
 *
 * @return A ByteVector containing the hexadecimal bytes values in ASCII.
 *
 * @details This function convert the following bytes `{0, 15, 16, 42, 19, 55}` to the following
 * bytes: `{0x30, 0x30, 0x30, 0x66, 0x31, 0x30, 0x32, 0x61, 0x31, 0x33, 0x33, 0x37} alternatively
 * said: `{'0', '0', '0', 'f', '1', '0', '2', 'a', '1', '3', '3', '7'}`.
 */
ByteVector to_ascii(const ByteVector & bytes, bool uppercase = false);

/**
 * @brief Convert byte to his hexadecimal value in ASCII.
 *
 * @param bytes      Byte to convert
 * @param uppercase  Convert to uppercase hexadecimal, or not
 *
 * @return A ByteVector containing the hexadecimal bytes values in ASCII.
 *
 * @details This function convert the following byte `42` to the following bytes: `{0x32, 0x61}`
 * alternatively said: `{'2', 'a'}`.
 */
ByteVector to_ascii(uint8_t byte, bool uppercase = false);

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

stm::ByteVector & operator << (stm::ByteVector & bv, const std::string & str);

stm::ByteVector & operator << (stm::ByteVector & bv, const stm::ByteVector & bv2);

stm::ByteVector & operator << (stm::ByteVector & bv, uint8_t byte);

stm::ByteVector & operator << (stm::ByteVector & bv, char ch);

stm::ByteVector & operator << (stm::ByteVector & bv, int value);

stm::ByteVector & operator << (stm::ByteVector & bv, unsigned int value);

stm::ByteVector & operator << (stm::ByteVector & bv, unsigned long value);

stm::ByteVector & operator << (stm::ByteVector & bv, GpsUtcTime timestamp);

template<typename T>
stm::ByteVector operator << (stm::ByteVector && bv, T data)
{
	return bv << data;
}

constexpr std::size_t operator"" _length(const char * , std::size_t N)
{
	return N;
}

#define BA(DATA) stm::utils::str2ba<DATA ## _length>(DATA)

#endif // TESEO_HAL_UTILS_BYTE_VECTOR_H