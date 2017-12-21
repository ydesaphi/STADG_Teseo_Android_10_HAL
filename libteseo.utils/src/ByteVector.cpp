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
 * @file ByteVector.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/utils/ByteVector.h>

#define LOG_TAG "teseo_hal_utils_ByteVector"
#include <cutils/log.h>

#include <stdexcept>
#include <typeinfo>
#include <sstream>

#include <teseo/utils/Time.h>

namespace stm {
namespace utils {

namespace impl {

constexpr const ByteArray<16> uppercase_hex_digits = BA("0123456789ABCDEF");

constexpr const ByteArray<16> lowercase_hex_digits = BA("0123456789abcdef");

constexpr ByteArray<2> to_ascii(uint8_t byte, bool uppercase)
{
	const ByteArray<16> & hex_digits = uppercase ? uppercase_hex_digits : lowercase_hex_digits;

	const uint8_t b1 = (byte & 0xF0) >> 4;
	const uint8_t b2 = byte & 0x0F;

	return { { hex_digits[b1], hex_digits[b2] } };
}

} // namespace impl

// Extracts a byte from a byte array at specified position, where the position is expressed in bit
void extract_byte(uint8_t &byte, const ByteVector & buffer, uint32_t pos)
{
	unsigned int byte_pos, bit_pos, i;
	uint8_t bit;

	byte_pos = pos / 8;
	bit_pos = pos % 8;
	byte = 0;

	for (i = 0; i < 8; i++)
	{
		if (i + bit_pos < 8)
		{
			bit = (buffer[byte_pos] & (1 << (7 - (i + bit_pos)))) != 0 ? 1 : 0;
		}
		else
		{
			bit = (buffer[byte_pos + 1] & (1 << (7 - (i + bit_pos - 8)))) != 0 ? 1 : 0;
		}
		byte += bit << (7 - i);
	}
}

// Extracts a 32-bit word from a byte array at specified position, where the position is expressed in bit
void extract_dword(uint32_t & dword, const ByteVector & buffer, uint32_t pos)
{
	ByteVector local_buffer(4);

	dword = 0;
	for (unsigned int i = 0; i <= 3; i++)
	{
		extract_byte(local_buffer[i], buffer, pos);
		pos += 8;
	}
	dword += local_buffer[0] * 256 * 256 * 256 + local_buffer[1] * 256 * 256 + local_buffer[2] * 256 + local_buffer[3];
}

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

ByteVector createFromString(const std::string & str)
{
	ByteVector vec;

	for(auto c : str)
	{
		if(c == '\0')
			break;

		vec.push_back(static_cast<uint8_t>(c));
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

static constexpr ByteArray<64>
b64_encode_table = BA("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

static constexpr uint8_t b64_pad_char = '=';

static constexpr ByteArray<256> b64_decode_table { {
    66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
} };

enum class b64_decode_char_type {
	Valid,
	Whitespace,
	PaddingChar,
	Invalid
};

constexpr b64_decode_char_type b64_decode_get_char_type(uint8_t c)
{
	return c  < 64 ? b64_decode_char_type::Valid       :
		   c == 64 ? b64_decode_char_type::Whitespace  :
		   c == 65 ? b64_decode_char_type::PaddingChar :
		 /*c == 66*/ b64_decode_char_type::Invalid;
}

ByteVector base64_decode(const std::string & base64)
{
	ByteVector bytes;

	uint32_t buffer = 0;
	uint8_t iteration = 0;

	for(auto encoded : base64)
	{
		uint8_t c = b64_decode_table[encoded];
		
		switch(b64_decode_get_char_type(c))
		{
		case b64_decode_char_type::Whitespace:  // Ignore whitespaces
		case b64_decode_char_type::PaddingChar: // Padding chars, we are done decoding here
			break;

		case b64_decode_char_type::Invalid:
			// Invalid characters, throw an exceptions
			throw std::runtime_error("Invalid character in base64");

		case b64_decode_char_type::Valid:
			// Valid char, let's decode
			buffer = buffer << 6 | static_cast<uint8_t>(c);
			iteration++;

			if(iteration == 4)
			{
				bytes.push_back((buffer >> 16) & 0xFF);
				bytes.push_back((buffer >> 8) & 0xFF);
				bytes.push_back(buffer & 0xFF);
				buffer = 0;
				iteration = 0;
			}

			break;
		}
	}

	if(iteration == 3)
	{
		bytes.push_back((buffer >> 10) & 0xFF);
		bytes.push_back((buffer >> 2) & 0xFF);
	}
	else if(iteration == 2)
	{
		bytes.push_back((buffer >> 4) & 0xFF);
	}

	return bytes;
}

std::string base64_encode(const ByteVector & bytes)
{
	std::string b64;
	b64.reserve(((bytes.size() / 3) + (bytes.size() % 3 > 0)) * 4);
	uint32_t buffer;

	auto ptr = bytes.begin();

	for(std::size_t i = 0; i < bytes.size() / 3; i++)
	{
		buffer = ((*ptr++) << 16) +
				 ((*ptr++) <<  8) +
				 ((*ptr++));
		
		b64.push_back(b64_encode_table[(buffer & 0x00FC0000) >> 18]);
		b64.push_back(b64_encode_table[(buffer & 0x0003F000) >> 12]);
		b64.push_back(b64_encode_table[(buffer & 0x00000FC0) >>  6]);
		b64.push_back(b64_encode_table[(buffer & 0x0000003F)      ]);
	}

	switch(bytes.size() % 3)
	{
		case 1:
			buffer = (*ptr) << 16;
			b64.push_back(b64_encode_table[(buffer & 0x00FC0000) >> 18]);
			b64.push_back(b64_encode_table[(buffer & 0x0003F000) >> 12]);
			b64.push_back(b64_pad_char);
			b64.push_back(b64_pad_char);
			break;

		case 2:
			buffer = ((*ptr++) << 16) +
			         ((*ptr)   <<  8);
			b64.push_back(b64_encode_table[(buffer & 0x00FC0000) >> 18]);
			b64.push_back(b64_encode_table[(buffer & 0x0003F000) >> 12]);
			b64.push_back(b64_encode_table[(buffer & 0x00000FC0) >>  6]);
			b64.push_back(b64_pad_char);
			break;
	}

	return b64;
}

ByteVector to_ascii(const ByteVector & bytes, bool uppercase)
{
	ByteVector ascii;
	ascii.reserve(bytes.size() * 2);

	for(uint8_t b : bytes)
	{
		auto bytes = impl::to_ascii(b, uppercase);
		ascii.push_back(bytes[0]);
		ascii.push_back(bytes[1]);
	}

	return ascii;
}

ByteVector to_ascii(uint8_t byte, bool uppercase)
{
	ByteArray<2> ascii = impl::to_ascii(byte, uppercase);
	return { ascii[0], ascii[1] };
}

} // namespace utils
} // namespace stm

stm::ByteVector & operator << (stm::ByteVector & bv, const std::string & str)
{
	for(auto c : str)
	{
		bv.push_back(c);
	}

	return bv;
}

stm::ByteVector & operator << (stm::ByteVector & bv, uint8_t byte)
{
	bv.push_back(byte);
	return bv;
}

stm::ByteVector & operator << (stm::ByteVector & bv, char ch)
{
	bv.push_back(ch);
	return bv;
}

stm::ByteVector & operator << (stm::ByteVector & bv, const stm::ByteVector & bv2)
{
	bv.insert(bv.end(), bv2.begin(), bv2.end());
	return bv;
}

stm::ByteVector & operator << (stm::ByteVector & bv, int value)
{
	return (bv << (std::ostringstream() << value).str());
}

stm::ByteVector & operator << (stm::ByteVector & bv, unsigned int value)
{
	return (bv << (std::ostringstream() << value).str());
}

stm::ByteVector & operator << (stm::ByteVector & bv, unsigned long value)
{
	return (bv << (std::ostringstream() << value).str());
}

stm::ByteVector & operator << (stm::ByteVector & bv, GpsUtcTime timestamp)
{
	return (bv << (std::ostringstream() << timestamp).str());
}
