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
};

} // namespace stm

#endif // TESEO_HAL_THREAD_H