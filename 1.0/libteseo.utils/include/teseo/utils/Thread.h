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
 * @brief Thread wrapper class
 * @file Thread.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_THREAD_H
#define TESEO_HAL_THREAD_H

#include <string>
#include <pthread.h>

#include "Signal.h"

namespace stm {
namespace priv {
void threadStart(void * rawThreadPtr);
} // namespace priv

/**
 * @brief      Thread wrapper class
 */
class Thread {
private:
	std::string name; ///< Thread name

	pthread_t handle; ///< Thread handle

	bool running; ///< Flag that indicates if the thread is running

	/**
	 * Thread create callback type
	 */
	typedef pthread_t (* CreateThreadCb)(const char* name, void (*start)(void *), void* arg);

	/**
	 * Thread create callback
	 */
	static CreateThreadCb createThread;

	friend void priv::threadStart(void *);

	/**
	 * Run function wrapper
	 *
	 * @details This function is called by the thread entry point. It is responsible of setting the
	 * `running` flag, emitting the `started` and `finished` signals and effectively call the `run`
	 * method.
	 */
	void runWrapper();

protected:
	/**
	 * @brief      Thread run function
	 * 
	 * @details    This is the function executed by the thread
	 */
	virtual void run() = 0;

public:
	/**
	 * @brief      Thread constructor
	 *
	 * @param[in]  name  The thread name
	 */
	Thread(const char * name);

	virtual ~Thread();

	/**
	 * @brief      Thread stop request method
	 *
	 * @return     0 on request success, 1 on failure
	 */
	virtual int stop() = 0;

	/**
	 * @brief      Pause until the thread ends
	 */
	void join();

	/**
	 * @brief      Start the thread
	 *
	 * @return     0 on success, 1 on failure
	 */
	int start();

	/**
	 * @brief      Register the create thread callback
	 *
	 * @param[in]  cb    The callback
	 */
	static void setCreateThreadCb(CreateThreadCb cb);

	/**
	 * @brief      Determines if the thread is running.
	 *
	 * @return     True if running, False otherwise.
	 */
	bool isRunning() const;

	/**
	 * Signal emitted when thread starts
	 */
	Signal<void> started;

	/**
	 * Signal emitted when thread stops
	 */
	Signal<void> finished;
};

} // namespace stm

#endif // TESEO_HAL_THREAD_H