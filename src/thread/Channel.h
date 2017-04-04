#ifndef TESEO_HAL_THREAD_CHANNEL
#define TESEO_HAL_THREAD_CHANNEL

#include <stdexcept>
#include <unistd.h>

#include "../errors.h"

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

template<typename Tdata>
class Channel {
private:
	int pipeInputFd;
	int pipeOutputFd;

	std::string name;

	bool opened;

	void openPipe()
	{
		int pipefd[2];

		opened = false;

		if(pipe(pipefd) != 0)
		{
			int err = errno;
			CHANNEL_LOGI("Error while opening channel '%s'", name.c_str())
			errors::pipe(err);
			return;
		}
		else
		{
			CHANNEL_LOGI("Channel '%s' opened successfully.", name.c_str());
		}

		pipeInputFd = pipefd[1];
		pipeOutputFd = pipefd[0];

		opened = true;
	}

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

	void send(const Tdata & data) noexcept
	{
		int ret = write(pipeInputFd, &data, sizeof(Tdata));

		if(ret == -1)
			errors::write(ret);
	}

	Tdata receive()
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
				throw std::system_error(errors::read(ret), std::system_category());
			else
			{
				CHANNEL_LOGE("Channel '%s' readed byte count is incoherent: %d read, %lu waited.", ret, sizeof(Tdata));
				throw std::runtime_error("Channel readed byte count is incoherent");
			}
		}
	}

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