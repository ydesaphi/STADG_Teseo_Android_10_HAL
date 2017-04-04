#ifndef TESEO_HAL_THREAD_H
#define TESEO_HAL_THREAD_H

#include <string>
#include <pthread.h>

namespace stm {
namespace priv {
void threadStart(void * rawThreadPtr);
} // namespace priv

class Thread {
private:
	std::string name;

	pthread_t handle;

	bool running;

	typedef pthread_t (* CreateThreadCb)(const char* name, void (*start)(void *), void* arg);

	static CreateThreadCb createThread;

	friend void priv::threadStart(void *);

protected:
	virtual void run() = 0;

public:
	Thread(const char * name);

	virtual ~Thread();

	virtual int stop() = 0;

	void join();

	int start();

	static void setCreateThreadCb(CreateThreadCb cb);

	bool isRunning() const;
};

} // namespace stm

#endif // TESEO_HAL_THREAD_H