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