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
#include <teseo/utils/IByteStream.h>

#define LOG_TAG "teseo_hal_ByteStream"
#include <cutils/log.h>

namespace stm {
namespace stream {

void ByteStreamReader::run()
{
	ByteStreamOpener<true> bsOpener(byteStream);

	if(!bsOpener)
	{
		ALOGE("Unable to open byte stream, ByteStreamReader will exit early.");
		return;
	}

	runReader = true;
	while(runReader)
	{
		auto bv = byteStream.perform_read();
		byteStream.newBytes(bv);
	}
}

ByteStreamReader::ByteStreamReader(IByteStream & bs) :
	Thread("ByteStreamReader"),
	byteStream(bs),
	runReader(true)
{ }

int ByteStreamReader::stop()
{
	runReader = false;
	return 0;
}

void ByteStreamWriter::run()
{
	ByteStreamOpener<true> bsOpener(byteStream);

	if(!bsOpener)
	{
		ALOGE("Unable to open byte stream, ByteStreamWriter will exit early.");
		return;
	}

	bool runWriter = true;
	while(runWriter)
	{
		switch(com.receive())
		{
			case WRITE:
				byteStream.perform_write(dataChannel.receive());
				break;

			case STOP:
				runWriter = false;
				break;
		}
	}
}

ByteStreamWriter::ByteStreamWriter(IByteStream & bs) :
	Thread("ByteStreamWriter"),
	byteStream(bs),
	com("ByteStreamWriter::com"),
	dataChannel("ByteStreamWriter::dataChannel")
{ }

int ByteStreamWriter::stop()
{
	com << STOP;
	return 0;
}

void ByteStreamWriter::write(const ByteVectorPtr bytes)
{
	dataChannel << bytes;
	com << WRITE;
}

AbstractByteStream::AbstractByteStream() :
	IByteStream(),
	reader(*this),
	writer(*this)
{ }

AbstractByteStream::~AbstractByteStream()
{
	if(writer.isRunning())
		writer.stop();

	if(reader.isRunning())
		reader.stop();
}

void AbstractByteStream::write(const ByteVectorPtr bytes)
{
	writer.write(bytes);
}

int AbstractByteStream::start()
{
	ALOGV("Start byte stream");
	int rw = writer.start();
	int rr = reader.start();
	return rr == 0 && rw == 0 ? 0 : 1;
}

int AbstractByteStream::stop()
{
	ALOGV("Stop byte stream");
	int rw = writer.stop();
	int rr = reader.stop();
	return rr == 0 && rw == 0 ? 0 : 1;
}

namespace __private_ByteStreamOpenerLog {
void loge(const char * format, std::string streamName, const char * what)
{
	ALOGE(format, streamName.c_str(), what);
}
} // namespace __private_ByteStreamOpenerLog

} // namespace stream
} // namespace stm