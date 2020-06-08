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
#ifndef TESEO_HAL_UTILS_UTILS_H
#define TESEO_HAL_UTILS_UTILS_H

#include <iterator>
#include <queue>
#include <string>
#include <sstream>

namespace stm {
namespace utils {

template<class Container, class UnaryPredicate>
void erase_if(Container & c, typename Container::iterator begin, typename Container::iterator end, UnaryPredicate p)
{
	while(begin != end)
	{
		if(p(*begin))
		{
			begin = c.erase(begin);
		}
		else
		{
			++begin;
		}
	}
}

template<class Container, class UnaryPredicate>
void erase_if(Container & c, UnaryPredicate p)
{
	erase_if(c, c.begin(), c.end(), p);
}

namespace impl {
template<class OutputValueType, class InputIterator, class SepType>
std::vector<OutputValueType> split(InputIterator start, InputIterator end, const SepType & sep)
{
	std::vector<OutputValueType> output;

	for(auto it = start; it != end; ++start)
	{
		if(*it == sep)
		{
			output.emplace_back(start, it);
			start = it + 1;
		}
	}

	if(start < end)
	{
		output.emplace_back(start, end);
	}

	return output;
}

template<class OutputValueType, class InputIterator, class Functor>
std::vector<OutputValueType> split_if(InputIterator begin, InputIterator end, Functor pred)
{
	std::vector<OutputValueType> output;

	for(auto it = begin; it != end; ++it)
	{
		if(pred(*it))
		{
			output.emplace_back(begin, it);
			begin = it + 1;
		}
	}

	if(begin < end)
	{
		output.emplace_back(begin, end);
	}

	return output;
}
}

template<class OutputValueType, class InputIterator, class SepType>
std::vector<OutputValueType> split(InputIterator begin, InputIterator end, const SepType & sep)
{
	return impl::split<OutputValueType, InputIterator, SepType>(begin, end, sep);
}

std::vector<std::string> split(
	std::string::const_iterator begin,
	std::string::const_iterator end,
	std::string::const_reference sep);

std::vector<std::string> split(
	std::string::iterator begin,
	std::string::iterator end,
	std::string::const_reference sep);

template<class OutputValueType, class InputIterator, class Functor>
auto split_if(InputIterator begin, InputIterator end, Functor pred)
{
	return impl::split_if<OutputValueType, InputIterator, Functor>(begin, end, pred);
}

template<class Functor>
std::vector<std::string> split_if(
	std::string::const_iterator begin,
	std::string::const_iterator end,
	Functor pred)
{
	return impl::split_if<
				std::string,
				std::string::const_iterator,
				Functor>(begin, end, pred);
}

template<class Functor>
std::vector<std::string> split_if(
	std::string::iterator begin,
	std::string::iterator end,
	Functor pred)
{
	return impl::split_if<
				std::string,
				std::string::iterator,
				Functor>(begin, end, pred);
}

template<class InputIterator>
std::string join(InputIterator begin, InputIterator end, std::string glue)
{
	if(begin == end)
		return std::string("");

	std::ostringstream oss;
	oss << *begin;

	for(auto it = begin + 1; it != end; ++it)
		oss << glue << *it;

	return oss.str();
}

} // namespace utils
} // namespace stm

#endif // TESEO_HAL_UTILS_UTILS_H