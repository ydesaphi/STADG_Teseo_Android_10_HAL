/*
 * This file is part of Teseo Android HAL
 *
 * Copyright (c) 2016-2020, STMicroelectronics - All Rights Reserved
 * Author(s): Baudouin Feildel <baudouin.feildel@st.com> for STMicroelectronics.
 *
 * License terms: Apache 2.0.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @brief Thread wrapper class
 * @file Thread.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#define LOG_TAG "teseo_hal_Thread"

#include <log/log.h>

#include <teseo/utils/Thread.h>

namespace stm {
namespace priv {

void threadStart(void * rawThreadPtr)
{
	static_cast<Thread*>(rawThreadPtr)->runWrapper();
}

} // namespace priv

Thread::CreateThreadCb Thread::createThread = nullptr;
std::vector<std::unique_ptr<Thread::ThreadFuncArgs>> sThreadFuncArgsList;

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

void* Thread::threadFunc(void* arg)
{
    Thread::ThreadFuncArgs* threadArgs = reinterpret_cast<Thread::ThreadFuncArgs*>(arg);
    threadArgs->fptr(threadArgs->args);
    return nullptr;
}

pthread_t Thread::createPthread(const char* name,
    void (*start)(void*),
    void* arg,
    std::vector<std::unique_ptr<Thread::ThreadFuncArgs>> * listArgs)
{
    pthread_t threadId;
    auto threadArgs = new Thread::ThreadFuncArgs(start, arg);
    auto argPtr = std::unique_ptr<Thread::ThreadFuncArgs>(threadArgs);

    listArgs->push_back(std::move(argPtr));

    int ret = pthread_create(&threadId,
                nullptr,
                Thread::threadFunc,
                reinterpret_cast<void*>(threadArgs));
    if (ret != 0) {
        ALOGE("pthread creation unsuccessful");
    } else {
        pthread_setname_np(threadId, name);
    }
    return threadId;
}

} // namespace stm
