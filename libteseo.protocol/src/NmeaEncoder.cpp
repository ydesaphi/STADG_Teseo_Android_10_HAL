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

#include <teseo/utils/Time.h>

using namespace stm::model;

namespace stm {
namespace protocol {

namespace messages {
constexpr const auto get_versions = BA("PSTMGETSWVER,255");

constexpr const auto stagps_password_generate = BA("PSTMSTAGPSPASSGEN");

constexpr const auto stagps8_password_generate = BA("PSTMSTAGPS8PASSGEN");

constexpr const auto stagps_realtime_ephemeris = BA("PSTMEPHEM");

constexpr const auto stagps_realtime_almanac = BA("PSTMALMANAC");

constexpr const auto stagps_pgps7_seed = BA("PSTMSTAGPSSATSEED");
} // namespace messages

template<std::size_t N>
ByteVectorPtr ba2bvptr(const ByteArray<N> & ba)
{
	return std::make_shared<ByteVector>(ba.begin(), ba.end());
}

namespace encoders {

template<std::size_t N>
ByteVectorPtr generic_encoder(
	const ByteArray<N> & msgId,
	std::size_t expected_parameters_count,
	const std::vector<ByteVector> & parameters)
{
	if(parameters.size() != expected_parameters_count)
	{
		/*ALOGE("Expected %u parameters, only %u received",
			expected_parameters_count,
			parameters.size());
			//*/
		throw std::runtime_error("Bad parameter count");
	}

	ByteVectorPtr messagePtr = std::make_shared<ByteVector>();
	ByteVector & message = *messagePtr;

	message.insert(message.begin(), msgId.begin(), msgId.end());

	for(const auto & p : parameters)
	{
		message << "," << p;
	}

	return messagePtr;
}

ByteVectorPtr get_versions(
	const device::AbstractDevice & device,
	const std::vector<ByteVector> & parameters)
{
	// Unused params
	(void)(device);
	(void)(parameters);

	return ba2bvptr(messages::get_versions);
}

ByteVectorPtr stagps_password_generate(
	const device::AbstractDevice &,
	const std::vector<ByteVector> & parameters)
{

	ALOGI("Encode ST-AGPS Password generate message");

	ByteVectorPtr messagePtr = std::make_shared<ByteVector>();
	ByteVector & message = *messagePtr;

	message.insert(message.begin(),
		messages::stagps_password_generate.begin(),
		messages::stagps_password_generate.end());

	auto t = utils::systemNow();
	message << ',' << utils::utc_timestamp_to_gps_timestamp(t) / 1000 << ',';

	if(parameters.size() != 2)
	{
		//ALOGE("Expected 2 parameters, only %u received", parameters.size());
		throw std::runtime_error("Expected 2 parameters");
	}

	auto vendorId = parameters.at(0);
	auto deviceId = parameters.at(1);

	message << vendorId << ','
			<< deviceId;

	return messagePtr;
}

ByteVectorPtr stagps8_password_generate(
	const device::AbstractDevice &,
	const std::vector<ByteVector> & parameters)
{
	ALOGI("Encode ST-AGPS 8 Password generate message");

	if(parameters.size() != 3)
	{
		//ALOGE("Expected 3 parameters, only %u received", parameters.size());
		throw std::runtime_error("Expected 3 parameters");
	}

	ByteVectorPtr messagePtr = std::make_shared<ByteVector>();
	ByteVector & message = *messagePtr;

	message.insert(message.begin(),
		messages::stagps8_password_generate.begin(),
		messages::stagps8_password_generate.end());

	auto t = utils::systemNow();
	message << ',' << utils::utc_timestamp_to_gps_timestamp(t) / 1000 << ',';

	auto vendorId = parameters.at(0);
	auto modelId  = parameters.at(1);
	auto deviceId = parameters.at(2);

	message << vendorId << ','
			<< modelId  << ','
			<< deviceId;

	return messagePtr;
}

ByteVectorPtr stagps_realtime_ephemeris(
	const device::AbstractDevice &,
	const std::vector<ByteVector> & parameters)
{
	ALOGI("Encode ST-AGPS Ephemeris injection message");
	return generic_encoder(messages::stagps_realtime_ephemeris, 3, parameters);
}

ByteVectorPtr stagps_realtime_almanac(
	const device::AbstractDevice &,
	const std::vector<ByteVector> & parameters)
{
	ALOGI("Encode ST-AGPS Ephemeris injection message");
	return generic_encoder(messages::stagps_realtime_almanac, 3, parameters);
}

ByteVectorPtr stagps_pgps7_seed(
	const device::AbstractDevice &,
	const std::vector<ByteVector> & parameters)
{
	ALOGI("Encode ST-AGPS pgps7 seed injection message");
	return generic_encoder(messages::stagps_pgps7_seed, 7, parameters);
}


} // namespace encoders

void NmeaEncoder::encode(
	const device::AbstractDevice & device,
	const model::Message & message)
{
	switch(message.id)
	{
		case MessageId::GetVersions:
			encodedBytes(encoders::get_versions(device, message.parameters));
			break;

		case MessageId::Stagps8_PasswordGenerate:
			encodedBytes(encoders::stagps8_password_generate(device, message.parameters));
			break;

		
		case MessageId::Stagps_PasswordGenerate:
			encodedBytes(encoders::stagps_password_generate(device, message.parameters));
			break;

		case MessageId::Stagps_RealTime_Ephemeris:
			encodedBytes(encoders::stagps_realtime_ephemeris(device, message.parameters));
			break;

		case MessageId::Stagps_RealTime_Almanac:
			encodedBytes(encoders::stagps_realtime_almanac(device, message.parameters));
			break;

		case MessageId::Stagps_PGPS7_Seed:
			encodedBytes(encoders::stagps_pgps7_seed(device, message.parameters));
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