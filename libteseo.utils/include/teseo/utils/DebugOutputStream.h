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

#ifndef TESEO_HAL_UTILS_DEBUG_OUTPUT_STREAM_H
#define TESEO_HAL_UTILS_DEBUG_OUTPUT_STREAM_H

#include <list>
#include <memory>
#include <netinet/in.h>
#include <cstdint>

#include "Signal.h"
#include "Thread.h"
#include "Channel.h"
#include "ByteVector.h"

namespace stm::debug {

#ifdef ENABLE_DEBUG_OUTPUT_STREAM
class TcpClientSocket {
private:
	int sockfd;

public:
	TcpClientSocket(int sockfd);

	void send(const ByteVector & data);

	bool opened();

	void close();
};

class TcpServerSocket : public Thread {
private:
	uint16_t port;
	int sockfd;
	sockaddr_in addr;

	std::list<std::weak_ptr<TcpClientSocket>> clients;

	bool open();

	void close();

protected:
	virtual void run();

public:
	TcpServerSocket(uint16_t port);

	virtual ~TcpServerSocket();

	Signal<void, std::shared_ptr<TcpClientSocket>> newClient;

	int stop();
};
#endif

#ifdef ENABLE_DEBUG_OUTPUT_STREAM
class DebugOutputStream : public Thread
#else
class DebugOutputStream
#endif
{
private:
// If debug output stream isn't enabled all private stuff won't be compiled
#ifdef ENABLE_DEBUG_OUTPUT_STREAM
	enum Control : uint8_t {
		APPEND = 0,
		CLEAR  = 1,
		SEND   = 2,
		STOP   = 3
	};

	struct Message {
		Control command;
		uint8_t data;
	};

	TcpServerSocket socket;
	std::list<std::shared_ptr<TcpClientSocket>> clients;

	thread::Channel<Message> com;

protected:
	void run();
#endif

public:
	DebugOutputStream(uint16_t port);

	virtual ~DebugOutputStream();

	int stop();

	void send(const char * data, std::size_t count);

	void send(const uint8_t * data, std::size_t count);

	void send(const ByteVector & data);

	void send(const std::string & data);

#ifdef ENABLE_DEBUG_OUTPUT_STREAM
	template<std::size_t N>
	void send(const char (&data)[N])
	{
		for(std::size_t i = 0; i < N; i++)
		{
			com << Message { Control::APPEND, data[i] };
		}

		com << Message { Control::SEND, 0 };
		com << Message { Control::CLEAR, 0 };
	}
#else
	template<std::size_t N>
	void send(const char (&)[N])
	{ }

	int start();

	void join();

	bool isRunning() const;
#endif
};

} // namespace stm::debug

#endif // TESEO_HAL_UTILS_DEBUG_OUTPUT_STREAM_H