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
#include <teseo/protocol/NmeaEncoder.h>

#include <utility>

#define LOG_TAG "teseo_hal_NmeaEncoder"
#include <cutils/log.h>

#include <teseo/vendor/frozen/unordered_map.h>
#include <teseo/utils/Time.h>

using namespace stm::model;

namespace stm {
namespace protocol {

namespace messages {
constexpr const auto get_versions = BA("PSTMGETSWVER,255");

constexpr const auto stagps_password_generate = BA("PSTMSTAGPSPASSWGEN");

constexpr const auto stagps8_password_generate = BA("PSTMSTAGPS8PASSGEN");
} // namespace messages

template<std::size_t N>
ByteVector ba2bv(const ByteArray<N> & ba)
{
	return ByteVector(ba.begin(), ba.end());
}

namespace encoders {
ByteVector get_versions(
	const device::AbstractDevice & device,
	const std::vector<ByteVector> & parameters)
{
	// Unused params
	(void)(device);
	(void)(parameters);

	return ba2bv(messages::get_versions);
}

ByteVector stagps_password_generate(
	const device::AbstractDevice & device,
	const std::vector<ByteVector> & parameters)
{
	ByteVector message;
	message.insert(message.begin(),
		messages::stagps_password_generate.begin(),
		messages::stagps_password_generate.end());

	if(auto timestamp = device.getTimestamp())
	{
		message << ',' << *timestamp << ',';
	}
	else
	{
		auto t = utils::systemNow();
		ALOGW("Timestamp wasn't received, use system timestamp: %s",
			utils::time2string(t).c_str());
		message << ',' << t << ',';
	}

	if(parameters.size() != 2)
	{
		ALOGE("Expected 2 parameters, only %lu received", parameters.size());
		throw std::runtime_error("Expected 2 parameters");
	}

	auto vendorId = parameters.at(0);
	auto deviceId = parameters.at(1);

	message << vendorId << ','
			<< deviceId;

	return message;
}

ByteVector stagps8_password_generate(
	const device::AbstractDevice & device,
	const std::vector<ByteVector> & parameters)
{
	ALOGI("Encode ST-AGPS 8 Password generate message");
	ByteVector message;

	message.insert(message.begin(),
		messages::stagps8_password_generate.begin(),
		messages::stagps8_password_generate.end());

	if(auto timestamp = device.getTimestamp())
	{
		message << ',' << *timestamp << ',';
	}
	else
	{
		auto t = utils::systemNow();
		ALOGW("Timestamp wasn't received, use system timestamp: %s",
			utils::time2string(t).c_str());
		message << ',' << t << ',';
	}

	if(parameters.size() != 3)
	{
		ALOGE("Expected 3 parameters, only %lu received", parameters.size());
		throw std::runtime_error("Expected 2 parameters");
	}

	auto vendorId = parameters.at(0);
	auto modelId  = parameters.at(1);
	auto deviceId = parameters.at(2);

	message << vendorId << ','
			<< modelId  << ','
			<< deviceId;

	return message;
}
} // namespace encoders

void NmeaEncoder::encode(
	model::MessageId id,
	const device::AbstractDevice & device,
	const std::vector<ByteVector> & parameters)
{
	switch(id)
	{
		case MessageId::GetVersions:
			fireEncodedBytes(encoders::get_versions(device, parameters));
			break;

		case MessageId::Stagps8_PasswordGenerate:
			fireEncodedBytes(encoders::stagps8_password_generate(device, parameters));
			break;

		
		case MessageId::Stagps_PasswordGenerate:
			fireEncodedBytes(encoders::stagps_password_generate(device, parameters));
			break;

		default:
			ALOGE("Message not supported by encoder.");
			break;
	}
}

NmeaEncoder::NmeaEncoder() :
	IEncoder()
{ }

NmeaEncoder::~NmeaEncoder()
{ }

} // namespace protocol
} // namespace stm