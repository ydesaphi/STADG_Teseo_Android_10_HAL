#ifndef TESEO_HAL_UTILS_LOG_H
#define TESEO_HAL_UTILS_LOG_H

#define LOG_NDEBUG 1

#include <sstream>
#include <cutils/log.h>

#include <teseo/utils/ByteVector.h>
#include <teseo/utils/Time.h>

namespace stm {
namespace log {

enum Operator {
	endl
};

enum class LogLevel {
	Verbose,
	Debug,
	Info,
	Warning,
	Error
};

template<LogLevel lvl>
void log_string(const std::string & str)
{
	(void)(str);
	throw std::runtime_error("Not overloaded for specific LogLevel value.");
}

template<>
void log_string<LogLevel::Verbose>(const std::string & str)
{
	ALOGV("%s", str.c_str());
}

template<>
void log_string<LogLevel::Debug>(const std::string & str)
{
	ALOGD("%s", str.c_str());
}

template<>
void log_string<LogLevel::Info>(const std::string & str)
{
	ALOGI("%s", str.c_str());
}

template<>
void log_string<LogLevel::Warning>(const std::string & str)
{
	ALOGW("%s", str.c_str());
}

template<>
void log_string<LogLevel::Error>(const std::string & str)
{
	ALOGE("%s", str.c_str());
}

#define LOGCPPSTYLE(data) do {              \
	std::ostringstream __logstream;         \
	__logstream << data;                    \
	ALOGI("%s", __logstream.str().c_str()); \
} while(0)

template<LogLevel lvl>
class Logger {
private:
	std::string buffer;

	friend Logger<lvl> & operator << (Logger<lvl> logger, const Operator & op);

public:

	Logger()
	{ }

	Logger(const Logger<lvl> & other) :
		buffer(other.buffer)
	{ }

	Logger(Logger<lvl> && other) :
		buffer(std::move(other.buffer))
	{ }

	template<typename T>
	Logger & operator << (const T & data)
	{
		std::stringstream ss;
		ss << data;

		buffer.append(ss.str());
	}
};

template<LogLevel lvl>
Logger<lvl> & operator << (Logger<lvl> & logger, const Operator & op)
{
	log_string<lvl>(logger.buffer);
	logger.buffer.clear();
	return logger;
}

Logger<LogLevel::Verbose> verbose() { return Logger<LogLevel::Verbose>(); }
Logger<LogLevel::Debug>   debug()   { return Logger<LogLevel::Debug>();   }
Logger<LogLevel::Info>    info()    { return Logger<LogLevel::Info>();    }
Logger<LogLevel::Warning> warning() { return Logger<LogLevel::Warning>(); }
Logger<LogLevel::Error>   error()   { return Logger<LogLevel::Error>();   }

} // namespace log
} // namespace stm

#endif // TESEO_HAL_UTILS_LOG_H