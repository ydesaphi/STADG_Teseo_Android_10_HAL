/**
 * @brief Provide asynchronous communication between threads
 * @file Channel.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_THREAD_CHANNEL
#define TESEO_HAL_THREAD_CHANNEL

#include <stdexcept>
#include <system_error>
#include <unistd.h>

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
template<typename Tdata>
class Channel {
private:
	int pipeInputFd;  ///< Channel input
	int pipeOutputFd; ///< Channel output

	std::string name; ///< Channel name

	bool opened;      ///< Flag that indicates if the channel is opened or not

	/**
	 * @brief      Opens a the channel pipe.
	 */
	void openPipe()
	{
		int pipefd[2];

		opened = false;

		if(pipe(pipefd) != 0)
		{
			int err = errno;
			CHANNEL_LOGI("Error while opening channel '%s'", name.c_str())
			throw std::system_error(errors::pipe(err), std::system_category());
		}
		else
		{
			CHANNEL_LOGI("Channel '%s' opened successfully.", name.c_str());
		}

		pipeInputFd = pipefd[1];
		pipeOutputFd = pipefd[0];

		opened = true;
	}

	/**
	 * @brief      Closes the channel pipe.
	 */
	void closePipe()
	{
		if(opened)
		{
			close(pipeInputFd);
			close(pipeOutputFd);
			CHANNEL_LOGI("Channel '%s' closed successfully.", name.c_str());
		}
	}

public:
	Channel(const char * name) :
		name(name)
	{
		openPipe();
	}

	~Channel()
	{
		closePipe();
	}

	/**
	 * @brief      Send data in the channel
	 *
	 * @param[in]  data  Data to send
	 */
	void send(const Tdata & data) throw(std::system_error)
	{
		int ret = write(pipeInputFd, &data, sizeof(Tdata));

		if(ret == -1)
		{
			int errno_ = errno;
			throw std::system_error(errors::write(errno_), std::system_category());
		}
	}

	/**
	 * @brief      Receive data from the channel
	 * 
	 * @details    If the channel is empty this method block the current thread until data is
	 * available.
	 *
	 * @return     Data received
	 * 
	 * @throws     std::system_error System error when the receive fails.
	 * @throws     std::runtime_error Runtime error if the received byte count is incoherent with
	 * the data size.
	 */
	Tdata receive() throw(std::system_error, std::runtime_error)
	{
		Tdata data;
		int ret = read(pipeOutputFd, &data, sizeof(Tdata));

		if(ret == sizeof(Tdata))
		{
			return data;
		}
		else
		{
			if(ret == -1)
			{
				int errno_ = errno;
				throw std::system_error(errors::read(errno_), std::system_category());
			}
			else
			{
				CHANNEL_LOGE(
					"Channel '%s' readed byte count is incoherent: %d read, %lu waited.",
					ret, sizeof(Tdata));
				throw std::runtime_error("Channel readed byte count is incoherent");
			}
		}
	}

	/**
	 * @brief      Receive data from the channel
	 *
	 * @details    If the channel is empty this method block the current thread until data is
	 * available.
	 * 
	 * @param      buffer  The destination buffer
	 *
	 * @return     The received byte count
	 */
	int receive(Tdata * buffer) noexcept
	{
		return read(pipeOutputFd, buffer, sizeof(Tdata));
	}
};

} // namespace thread
} // namespace stm

#ifdef UNDEF_LOG_TAG
#undef LOG_TAG
#endif

#endif // TESEO_HAL_THREAD_CHANNEL