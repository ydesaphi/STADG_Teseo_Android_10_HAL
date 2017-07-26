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
 */
std::string bytesToString(const ByteVector & bytes);

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
 */
std::string bytesToString(const ByteVector::const_iterator & start, const ByteVector::const_iterator & end);

template<typename Tout>
struct ByteVectorParser
{
	std::optional<Tout> operator()(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
	{
		(void)(begin); (void)(end);
		static_assert(true, "Missing implementation of ByteVectorParser");
		return {};
	}

	std::optional<Tout> operator()(const ByteVector & data)
	{
		(void)(data);
		static_assert(true, "Missing implementation of ByteVectorParser");
		return {};		
	}
};

template<>
struct ByteVectorParser<int>
{
	std::optional<int> operator()(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end)
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

template <typename Tout, class Parser=ByteVectorParser<Tout> >
std::optional<Tout> byteVectorParse(
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

template <typename Tout, class Parser=ByteVectorParser<Tout> >
std::optional<Tout> byteVectorParse(const ByteVector & value)
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
 */
ByteVector createFromString(const char * str);

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

constexpr std::size_t operator"" _length(const char * , std::size_t N)
{
	return N;
}

#define BA(DATA) stm::utils::str2ba<DATA ## _length>(DATA)

#endif // TESEO_HAL_UTILS_BYTE_VECTOR_H