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
#include <teseo/utils/http.h>

#define LOG_TAG "teseo_hal_http"
#include <cutils/log.h>

#include <sstream>
#include <curl/curl.h>

#include <teseo/utils/DebugOutputStream.h>

namespace stm {
namespace utils {

#ifdef DEBUG_HTTP_CLIENT
debug::DebugOutputStream * dbgHttp = nullptr;
#endif

void http_init()
{
	curl_global_init(CURL_GLOBAL_ALL);
	#ifdef DEBUG_HTTP_CLIENT
	dbgHttp = new debug::DebugOutputStream(13372);
	dbgHttp->start();
	#endif
}

void http_cleanup()
{
	curl_global_cleanup();
	#ifdef DEBUG_HTTP_CLIENT
	dbgHttp->stop();
	delete dbgHttp;
	dbgHttp = nullptr;
	#endif
}

struct curl_slist * headers_to_curl_slist(const std::vector<Header> & headers)
{
	struct curl_slist * dest = nullptr;

	for(Header h : headers)
	{
		dest = curl_slist_append(dest, 
			(std::ostringstream() << h.name << ": " << h.value).str().c_str());
	}

	return dest;
}

size_t curl_write_callback(char * ptr, size_t size, size_t nmemb, void * userdata)
{
	HttpRequest * req = static_cast<HttpRequest *>(userdata);
	size_t i = 0;

	if(req->stopped == true)
	{
		return 0;
	}
	
	#ifdef DEBUG_HTTP_CLIENT
		dbgHttp->send(ptr, size * nmemb);
	#endif

	while(i < size * nmemb)
	{
		req->response.content.push_back(ptr[i]);
		i++;
	}

	return i;
}

HttpRequest::HttpRequest() :
	Thread("http-request"),
	verb(Verb::GET),
	stopped(false)
{ }

HttpRequest & HttpRequest::setVerb(Verb v)
{
	if(!isRunning())
	{
		verb = v;
	}
	else
	{
		ALOGW("Trying to change HTTP request verb while executing request.");
	}

	return *this;
}

HttpRequest & HttpRequest::setUri(const std::string & u)
{
	if(!isRunning())
	{
		uri = u;
	}
	else
	{
		ALOGW("Trying to change HTTP request verb while executing request.");
	}
	
	return *this;
}

HttpRequest & HttpRequest::setUserAgent(const std::string & ua)
{
	if(!isRunning())
	{
		userAgent = ua;
	}
	else
	{
		ALOGW("Trying to change HTTP request user-agent while executing request.");
	}
	
	return *this;
}

HttpRequest & HttpRequest::addHeader(const std::string & name, const std::string & value)
{
	if(!isRunning())
	{
		headers.push_back({name, value});
	}
	else
	{
		ALOGW("Trying to change HTTP request verb while executing request.");
	}
	
	return *this;
}

HttpRequest & HttpRequest::setContent(const std::string & c)
{
	if(!isRunning())
	{
		content = c;
	}
	else
	{
		ALOGW("Trying to change HTTP request verb while executing request.");
	}
	
	return *this;
}

const HttpResponse & HttpRequest::getResponse() const
{
	return response;
}

int HttpRequest::stop()
{
	stopped = true;
	return 0;
}

void HttpRequest::run()
{
	CURL * curlHandle = curl_easy_init();

	if(curlHandle == nullptr)
	{
		response.statusCode = HttpStatusCode::CurlHandleIsNull;
		response.statusMessage = "cURL Handle is null";
		return;
	}
		
	CURLcode curlResponse;
	struct curl_slist * headers_slist = headers_to_curl_slist(headers);

	// == Set cURL options ==
	// Request uri and headers
	curl_easy_setopt(curlHandle, CURLOPT_URL, uri.c_str());
	curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, userAgent.c_str());
	curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headers_slist);

	if(verb == HttpRequest::POST)
	{
		ALOGD("Setting request verb to POST");
		ALOGD("Setting payload to: %s", content.c_str());
		curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, content.c_str());
	}
	else
	{
		ALOGD("Not a POST request.");
	}
	// If we need to support HTTP verbs we should switch the verb
	// to set the correct cURL options here.
	//
	//else if(verb != HttpRequest::GET)
	//{
	//	switch(verb)
	//	{
	//	case HttpRequest::PUT:
	//		...
	//	}
	//}

	// HTTP options
	curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1); // Follow redirects
	curl_easy_setopt(curlHandle, CURLOPT_AUTOREFERER, 1);    // Set referer on redirects

	// System options
	curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1); // Avoid signals to be raised by cURL

	// Register the write function and the write destination
	curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, curl_write_callback);
	curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, static_cast<void *>(this));
	// == End of cURL options ==

	// == Perform the request ==
	if(stopped)
	{
		response.statusCode = HttpStatusCode::StoppedBeforeRequest;
		response.statusMessage = "Stopped before request";
		curl_easy_cleanup(curlHandle);
		return;
	}

	#ifdef DEBUG_HTTP_CLIENT
		ALOGD("Perform HTTP request: %s %s",
			verb == HttpRequest::GET  ? "GET"  :
			verb == HttpRequest::POST ? "POST" : "UnknownVerb",
			uri.c_str()
		);

		dbgHttp->send("\r\nNew request\r\n");
		dbgHttp->send(verb == HttpRequest::GET  ? "GET "  :
					 verb == HttpRequest::POST ? "POST " : "UnknownVerb ");
		dbgHttp->send(uri);
		dbgHttp->send("\r\n");


		ALOGD("Headers:");
		ALOGD("User-Agent: %s", userAgent.c_str());
		dbgHttp->send("User-Agent: ");
		dbgHttp->send(userAgent);
		dbgHttp->send("\r\n");
		
		for(auto h : headers)
		{
			ALOGD("%s: %s", h.name.c_str(), h.value.c_str());
			dbgHttp->send(h.name);
			dbgHttp->send(": ");
			dbgHttp->send(h.value);
			dbgHttp->send("\r\n");
		}

		if(verb == HttpRequest::POST)
		{
			ALOGD("Payload: %s", content.c_str());
			dbgHttp->send("\r\n");
			dbgHttp->send(content);
		}
		else
		{
			ALOGD("Request without payload");
			dbgHttp->send("\r\n");
		}
	#endif

	curlResponse = curl_easy_perform(curlHandle);
	
	// == Extract response ==
	if(curlResponse != CURLE_OK)
	{
		ALOGE("cURL error: %s", curl_easy_strerror(curlResponse));

		response.statusCode = HttpStatusCode::InternalError;
		response.statusMessage = "cURL error: ";
		response.statusMessage += curl_easy_strerror(curlResponse);
	}
	else
	{
		long responseCode;
		curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &responseCode);
		response.statusCode = static_cast<HttpStatusCode>(responseCode);
	}

	curl_easy_cleanup(curlHandle);
}

} // namespace stm
} // namespace utils