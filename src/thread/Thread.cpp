/**
 * @brief Thread wrapper class
 * @file Thread.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include "Thread.h"

#define LOG_TAG "teseo_hal_Thread"
#include <cutils/log.h>

namespace stm {
namespace priv {

void threadStart(void * rawThreadPtr)
{
	Thread * thread = static_cast<Thread*>(rawThreadPtr);
	thread->running = true;
	thread->run();
	thread->running = false;
}

} // namespace priv

Thread::CreateThreadCb Thread::createThread = nullptr;

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
	handle = createThread(name.c_str(), &priv::threadStart, this);
	return handle != 0;
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