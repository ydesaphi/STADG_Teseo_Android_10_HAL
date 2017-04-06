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

/**
 * @brief      Parse integer in an ascii bytes vector
 *
 * @param[in]  bytes  The ascii bytes
 *
 * @return     Parsed integer
 * 
 * @throws     If the string is malformed
 */
int byteVectorParseInt(const ByteVector & bytes);

/**
 * @brief      Parse integer in an ascii bytes vector
 *
 * @param[in]  start  Byte vector start
 * @param[in]  end    Byte vector end
 *
 * @return     Parsed integer
 * 
 * @throws     If the string is malformed
 */
int byteVectorParseInt(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end);

/**
 * @brief      Parse double in an ascii bytes vector
 *
 * @param[in]  bytes  The ascii bytes
 *
 * @return     Parsed double
 * 
 * @throws     If the string is malformed
 */
double byteVectorParseDouble(const ByteVector & bytes);

/**
 * @brief      Parse double in an ascii bytes vector
 *
 * @param[in]  start  Byte vector start
 * @param[in]  end    Byte vector end
 *
 * @return     Parsed double
 * 
 * @throws     If the string is malformed
 */
double byteVectorParseDouble(const ByteVector::const_iterator & begin, const ByteVector::const_iterator & end);

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