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
#ifndef TESEO_HAL_MODEL_VALUE_CONTAINER_H
#define TESEO_HAL_MODEL_VALUE_CONTAINER_H

#include <teseo/utils/optional.h>

namespace stm {

enum class ValueStatus {
	AVAILABLE,
	NOT_AVAILABLE,
	WAITING_FOR_DATA
};

template <typename T>
class ValueContainer {
private:
	T value;
	ValueStatus status;

public:
	ValueContainer(const T & value) :
		value(value),
		status(ValueStatus::AVAILABLE)
	{ }

	ValueContainer() :
		status(ValueStatus::NOT_AVAILABLE)
	{ }

	void invalidate()
	{
		status = ValueStatus::NOT_AVAILABLE;
	}

	void requesting()
	{
		status = ValueStatus::WAITING_FOR_DATA;
	}

	void set(const T & newValue)
	{
		value = newValue;
		status = ValueStatus::AVAILABLE;
	}

	ValueStatus getStatus() const
	{
		return status;
	}

	bool isWaitingForData() const
	{
		return status == ValueStatus::WAITING_FOR_DATA;
	}

	operator bool() const
	{
		return status == ValueStatus::AVAILABLE;
	}

	      T & getValue()            { return value; }
	const T & getValue()      const { return value; }
	const T & getConstValue() const { return value; }

	T & operator * () { return value; }
	T * operator-> () { return &value; }

	const T & operator * () const { return value; }
	const T * operator-> () const { return &value; }

	operator       T ()       { return value; }
	operator const T () const { return value; }

	ValueContainer & operator = (const T & other)
	{
		set(other);
		return *this;
	}
};

} // namespace stm

#endif // TESEO_HAL_MODEL_DATA_STATUS_H