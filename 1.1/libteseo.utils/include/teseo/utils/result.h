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
#ifndef TESEO_HAL_UTILS_RESULT_H
#define TESEO_HAL_UTILS_RESULT_H

#include "optional.h"

namespace stm {

template<typename TData, typename TErr>
class Result {
public:
	enum Status {
		Ok,
		Err
	};

private:
	std::optional<TData> _data;
	std::optional<TErr> _error;
	Status _status;

public:
	Result(const TData & data) :
		_data(data),
		_error(),
		_status(Ok)
	{ }

	Result(const TErr & err) :
		_data(),
		_error(err),
		_status(Err)
	{ }

	Status status() const
	{
		return _status;
	}

	operator bool() const
	{
		return _status == Ok;
	}

	TData & data()        { return *_data; }
	TData & operator * () { return *_data; }
	TData * operator-> () { return _data.operator->(); }

	const TData & data()        const { return *_data; }
	const TData & operator * () const { return *_data; }
	const TData * operator-> () const { return _data.operator->(); }

	const TErr & error() const { return *_error; }
};

} // namespace stm

#endif // TESEO_HAL_UTILS_RESULT_H