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
 * @file Thread.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/utils/Thread.h>

#define LOG_TAG "teseo_hal_Thread"
#include <cutils/log.h>

namespace stm {
namespace priv {

void threadStart(void * rawThreadPtr)
{
	static_cast<Thread*>(rawThreadPtr)->runWrapper();
}

} // namespace priv

Thread::CreateThreadCb Thread::createThread = nullptr;

void Thread::runWrapper()
{
	running = true;
	started.emit();
	run();
	running = false;
	finished.emit();
}

void Thread::join()
{
	pthread_join(handle, NULL);
}

Thread::Thread(const char * name) :
	name(name)
{
	running = false;
}

Thread::~Thread()
{
	if(isRunning())
	{
		ALOGW("Thread %s is being deleted while running.", name.c_str());
		// Stop can't be called here as it is a pure virtual method
	}
}

int Thread::start()
{
	if(!running)
	{
		handle = createThread(name.c_str(), &priv::threadStart, this);
		return handle != 0;
	}
	else
	{
		ALOGW("Try to start a running thread");
		return 1;
	}
}

void Thread::setCreateThreadCb(CreateThreadCb cb)
{
	createThread = cb;
}

bool Thread::isRunning() const
{
	return running;
}

} // namespace stm