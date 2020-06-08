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
#ifndef TESEO_HAL_UTILS_HTTP_H
#define TESEO_HAL_UTILS_HTTP_H

#include <vector>
#include <unordered_map>
#include <functional>

#include "result.h"
#include "Thread.h"

#define USER_AGENT "hal-http-client"

namespace stm {
namespace utils {

enum class HttpStatusCode {
	Success                     = 200,
	MovedPermanently            = 301,
	Found                       = 302,
	NotModified                 = 304,
	BadRequest                  = 400,
	Unauthorized                = 401,
	Forbidden                   = 403,
	NotFound                    = 404,
	ProxyAuthenticationRequired = 407,
	InternalServerError         = 500,
	NotImplemented              = 501,
	BadGateway                  = 502,
	ServiceUnavailable          = 503,
	GatewayTimeout              = 504,

	// Error codes specific to the HAL
	InternalError               = 601,
	CurlHandleIsNull            = 602,
	StoppedBeforeRequest        = 603
};

struct HttpResponse {
	HttpStatusCode statusCode;
	std::string statusMessage;

	std::string content;
	std::vector<char> buffer;
};

struct Header {
	std::string name;
	std::string value;
};

class HttpRequest : public Thread {
public:
	enum Verb {
		GET,
		POST
	};

	HttpRequest();

	HttpRequest & setVerb(Verb verb);

	HttpRequest & setUri(const std::string & uri);

	HttpRequest & setUserAgent(const std::string & ua);

	HttpRequest & addHeader(const std::string & name, const std::string & value);

	HttpRequest & setContent(const std::string & content);

	const HttpResponse & getResponse() const;

	virtual int stop();

protected:
	virtual void run();

private:

	friend size_t curl_write_callback(char *, size_t, size_t, void *);
    friend size_t curl_write_response_cb(char *ptr, size_t size, size_t nmemb, void *data);

	Verb verb;
	std::string uri;
	std::vector<Header> headers;
	std::string userAgent;
	std::string content;
	bool stopped;
	HttpResponse response;
};

/**
 * Initialize the HTTP utils library
 */
void http_init();

/**
 * Cleanup the HTTP utils library
 */
void http_cleanup();

} // namespace utils
} // namespace stm

#endif // TESEO_HAL_UTILS_HTTP_H