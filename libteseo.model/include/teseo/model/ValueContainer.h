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