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
 * @brief Provide asynchronous communication between threads
 * @file Channel.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_THREAD_CHANNEL
#define TESEO_HAL_THREAD_CHANNEL

#include <type_traits>
#include <queue>
#include <list>


#include "errors.h"

#ifdef CHANNEL_DEBUG
	#ifndef LOG_TAG
		#define LOG_TAG "teseo_hal_Channel"
		#define UNDEF_LOG_TAG
	#endif

	#include <cutils/log.h>
	#define CHANNEL_LOGI(...) ALOGI(__VA_ARGS__)
	#define CHANNEL_LOGE(...) ALOGE(__VA_ARGS__)
	#define CHANNEL_LOGW(...) ALOGW(__VA_ARGS__)
#else
	#define CHANNEL_LOGI(...)
	#define CHANNEL_LOGE(...)
	#define CHANNEL_LOGW(...)
#endif

namespace stm {
namespace thread {

/**
 * @brief      Asynchronous typed data communication channel for threads
 * 
 * @details    This communication channel can be used to send data asynchronously from one thread to
 * another. This is a one to one communicaton channel. The sent data is stored in a FIFO list. If
 * there is no data in the FIFO, the reception is a blocking a operation.
 * Internally the channel use an anonymous pipe to send data.
 * 
 * If you want to send class instances through the channel you should only send pointer to instance.
 * Not the entire object, because the pipe buffer is limited and data bigger than this buffer is not
 * transmitted atomically.
 * 
 * The AbstractDecoder class is a good example on how to use a channel to send complex objects from
 * a thread to another.
 *
 * @tparam     Tdata  Data type
 */
template<typename T>
class Channel
{
public:

	static_assert(!std::is_reference<T>::value,
		"Channel doesn't works with references.");

	using Tval = typename
		std::conditional<std::is_const<T>::value, typename std::remove_cv<T>::type, T>::type;

	using Tconst_val = typename
		std::conditional<std::is_const<T>::value, T, typename std::add_const<T>::type>::type;

	using Tlvalue_ref = typename std::add_lvalue_reference<Tval>::type;

	using Tconst_lvalue_ref = typename
		std::conditional<std::is_const<T>::value,        // If (T is const) add reference to T
			typename std::add_lvalue_reference<T>::type, // else add const and reference to T
			typename std::add_const<typename std::add_lvalue_reference<T>::type>::type>::type;

	using Trvalue_ref = typename std::add_rvalue_reference<Tval>::type;
	
private:
	std::string name; ///< Channel name

	std::mutex mutex;
	std::condition_variable cond;

	std::queue<T, std::list<T>> queue;

public:

	Channel(const char * name) :
		name(name)
	{ }

	std::size_t size() const
	{
		return queue.size();
	}

	void clear()
	{
		std::unique_lock<std::mutex> lock(mutex);
		while(!queue.empty()) queue.pop();
	}

	/**
	 * @brief      Send data in the channel
	 *
	 * @param[in]  data  Data to send
	 */
	void send(Tconst_lvalue_ref data)
	{
		{
			std::unique_lock<std::mutex> lock(mutex);
			queue.push(data);
		}

		cond.notify_one();
	}

	void send(Trvalue_ref data)
	{
		send(data);
	}

	/**
	 * @brief      Receive data from the channel
	 * 
	 * @details    If the channel is empty this method block the current thread until data is
	 * available.
	 *
	 * @return     Data received
	 */
	T receive()
	{
		std::unique_lock<std::mutex> lock(mutex);

		if(queue.empty())
			cond.wait(lock, [this] { return !this->queue.empty(); });

		T data = queue.front();
		queue.pop();

		return data;
	}

	Channel & operator << (Tconst_lvalue_ref data)
	{
		send(data);
		return *this;
	}

	Channel & operator << (Trvalue_ref data)
	{
		send(data);
		return *this;
	}

	Channel & operator >> (Tlvalue_ref data)
	{
		data = receive();
		return *this;
	}
};

} // namespace thread
} // namespace stm

#ifdef UNDEF_LOG_TAG
#undef LOG_TAG
#endif

#endif // TESEO_HAL_THREAD_CHANNEL