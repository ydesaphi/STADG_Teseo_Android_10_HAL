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
#include <teseo/model/Almanac.h>

stm::ByteVector operator << (stm::ByteVector && bv, const stm::model::GpsAlmanac & alm)
{
	return (bv << alm);
}

stm::ByteVector & operator << (stm::ByteVector & bv, const stm::model::GpsAlmanac & alm)
{
	for(std::size_t i = 0; i < sizeof(stm::model::GpsAlmanacData); i++)
		bv.push_back(alm.raw[i]);

	return bv;
}