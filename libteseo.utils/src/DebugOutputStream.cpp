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

#include <teseo/utils/DebugOutputStream.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define LOG_TAG "teseo_hal_debugOutputStream"
#include <cutils/log.h>

#include <teseo/utils/ByteVector.h>

namespace stm::debug {

#ifdef ENABLE_DEBUG_OUTPUT_STREAM
TcpClientSocket::TcpClientSocket(int sockfd) :
	sockfd(sockfd)
{ }

void TcpClientSocket::send(const ByteVector & data)
{
	if(!opened())
	{
		ALOGE("Socket is closed, can't write to it.");
		return;
	}

	ssize_t res = write(sockfd, data.data(), data.size());
	if(res < 0)
	{
		ALOGE("Error while writing to socket. Close it.");
		close();
	}
}

bool TcpClientSocket::opened()
{
	return sockfd > 0;
}

void TcpClientSocket::close()
{
	if(opened())
	{
		::close(sockfd);
		sockfd = -1;
	}
}

bool TcpServerSocket::open()
{
	if(sockfd >= 0)
		return true;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		ALOGE("Can't open socket");
		return false;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) < 0)
	{
		ALOGE("Can't bind socket to port: %d", port);
		::close(sockfd);
		sockfd = -1;
		return false;
	}

	return true;
}

void TcpServerSocket::close()
{
	if(sockfd >= 0)
	{
		::close(sockfd);
		sockfd = -1;
	}
}

void TcpServerSocket::run()
{
	if(open())
	{
		bool stopListening = false;
		struct sockaddr clientAddr;
		socklen_t sockLen = sizeof(struct sockaddr);
		memset(&clientAddr, 0, sizeof(struct sockaddr));

		listen(sockfd, 5);

		while(!stopListening)
		{
			int clientSockFd = accept(sockfd, (struct sockaddr *) &clientAddr, &sockLen);
			if(clientSockFd < 1)
			{
				stopListening = true;
				ALOGE("Error while accepting client, stop listening");
			}
			else
			{
				auto clientPtr = std::make_shared<TcpClientSocket>(clientSockFd);
				ALOGV("New client connected");
				// Save weak_ptr to new client
				clients.push_back(clientPtr);
				// Signal new client
				newClient(clientPtr);
			}
		}

		close();
	}
	else
	{
		ALOGE("Error while opening socket");
	}
}

TcpServerSocket::TcpServerSocket(uint16_t port) :
	Thread("TcpServerSocket")
{
	this->port = port;
	this->sockfd = -1;
}

TcpServerSocket::~TcpServerSocket()
{
	stop();
	join();
}

int TcpServerSocket::stop()
{
	if(isRunning())
	{
		// Disconnect clients
		for(auto clientPtr : clients)
		{
			if(!clientPtr.expired())
			{
				clientPtr.lock()->close();
			}
		}

		// Shutdown socket
		shutdown(sockfd, SHUT_RDWR);
	}

	return 0;
}

void DebugOutputStream::run()
{
	if(!socket.start())
	{
		ALOGE("Error while opening socket exiting");
		return;
	}

	ByteVector buffer;
	bool stopLoop = false;
	Message msg;
	int i = 0;

	while(!stopLoop)
	{
		com >> msg;

		switch(msg.command)
		{
			case Control::SEND:
				i = 0;
				for(auto client: clients)
				{
					if(client->opened())
						client->send(buffer);
				}
				break;

			case Control::CLEAR:
				buffer.clear();
				break;

			case Control::STOP:
				stopLoop = true;
				break;

			case Control::APPEND:
				buffer.push_back(msg.data);
				break;
		}
	}

	socket.stop();
	socket.join();
}

DebugOutputStream::DebugOutputStream(uint16_t port) :
	Thread("DebugOutputStream"),
	socket(port),
	com("DebugOutputStream::com")
{
	socket.newClient.connect(SlotFactory::create(
		std::function<void(std::shared_ptr<TcpClientSocket>)>(
			[this] (std::shared_ptr<TcpClientSocket> client) {
				this->clients.push_back(client);
			})
		)
	);
}

DebugOutputStream::~DebugOutputStream()
{
	stop();
	join();
}

int DebugOutputStream::start()
{
	return static_cast<Thread *>(this)->start();
}

int DebugOutputStream::stop()
{
	if(isRunning())
	{
		com << Message { Control::STOP, 0 };
	}

	return 0;
}

void DebugOutputStream::send(const char * data, std::size_t count)
{
	for(std::size_t i = 0; i < count; i++)
	{
		com << Message { Control::APPEND, static_cast<uint8_t>(data[i]) };
	}
	
	com << Message { Control::SEND, 0 };
	com << Message { Control::CLEAR, 0 };
}

void DebugOutputStream::send(const uint8_t * data, std::size_t count)
{
	for(std::size_t i = 0; i < count; i++)
	{
		com << Message { Control::APPEND, data[i] };
	}
	
	com << Message { Control::SEND, 0 };
	com << Message { Control::CLEAR, 0 };
}

void DebugOutputStream::send(const ByteVector & data)
{
	for(uint8_t b : data)
	{
		com << Message { Control::APPEND, b };
	}
	
	com << Message { Control::SEND, 0 };
	com << Message { Control::CLEAR, 0 };
}

void DebugOutputStream::send(const std::string & data)
{
	for(auto ch: data)
	{
		com << Message { Control::APPEND, ch };
	}

	com << Message { Control::SEND, 0 };
	com << Message { Control::CLEAR, 0 };
}

#else // ifdef ENABLE_DEBUG_OUTPUT_STREAM
// Empty implementation of DebugOutputStream
DebugOutputStream::DebugOutputStream(uint16_t)
{ }

DebugOutputStream::~DebugOutputStream()
{ }

int DebugOutputStream::start()
{
	return 0;
}

int DebugOutputStream::stop()
{
	return 0;
}

void DebugOutputStream::send(const char *, std::size_t)
{ }

void DebugOutputStream::send(const uint8_t *, std::size_t)
{ }

void DebugOutputStream::send(const ByteVector &)
{ }

void DebugOutputStream::send(const std::string &)
{ }

void DebugOutputStream::join()
{ }
	
bool DebugOutputStream::isRunning() const
{
	return false;
}
#endif // ifdef ENABLE_DEBUG_OUTPUT_STREAM


} // namespace stm::debug
