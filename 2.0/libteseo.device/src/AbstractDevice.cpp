/*
 * This file is part of Teseo Android HAL
 *
 * Copyright (c) 2016-2020, STMicroelectronics - All Rights Reserved
 * Author(s): Baudouin Feildel <baudouin.feildel@st.com> for STMicroelectronics.
 *
 * License terms: Apache 2.0.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file AbstractDevice.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/device/AbstractDevice.h>

#define LOG_TAG "teseo_hal_AbstractDevice"
#include <log/log.h>
#include <time.h>

#include <teseo/utils/Wakelock.h>
#include <teseo/model/NmeaMessage.h>
#include <teseo/model/Message.h>

using namespace stm::model;

namespace stm {
namespace device {

const ByteVector AbstractDevice::nmeaSequenceStart {'G', 'G', 'A'};

AbstractDevice::AbstractDevice()
{ }

void AbstractDevice::init()
{
	statusUpdate(GnssStatusValue::NONE);
	gnssConstMask = 0;
}

void AbstractDevice::setLocation(const Location & loc)
{
	location.set(loc);
}

void AbstractDevice::addSatellite(const SatInfo & sat)
{
	if(!satellites)
		satellites.set(std::map<SatIdentifier, SatInfo>());

	auto & sats = *satellites;
	sats[sat.getId()] = sat;
}

void AbstractDevice::clearSatelliteList()
{
	satellites->clear();
	satellites.invalidate();
}

DrInfo & AbstractDevice::getDrInfo()
{
	return this->drInfo;
}

Result<GnssUtcTime, ValueStatus> AbstractDevice::getTimestamp() const
{
	if(this->timestamp)
		return *timestamp;

	return timestamp.getStatus();
}

Result<Location, ValueStatus> AbstractDevice::getLocation() const
{
	if(this->location)
		return *location;

	return location.getStatus();
}

Result<SatInfo, ValueStatus> AbstractDevice::getSatellite(const SatIdentifier & id) const
{
	if(!satellites)
		return ValueStatus::NOT_AVAILABLE;

	auto & sats = *satellites;
	auto it = sats.find(id);

	if(it != sats.end())
	{
		return it->second;
	}
	else
	{
		return ValueStatus::NOT_AVAILABLE;
	}
}

void AbstractDevice::update()
{
	// Update location only if it is valid
	if(location->locationValidity())
		locationUpdate(location);

	// Trigger satellite list update
	satelliteListUpdate(this->satellites);
}

void AbstractDevice::updateIfStartSentenceId(const ByteVector & sentenceId)
{
	if(sentenceId == nmeaSequenceStart)
	{
		// Trigger updates
		update();

		// Clear data before starting new sequence
		this->clearSatelliteList();
	}
}

int AbstractDevice::start()
{
	ALOGI("Start navigation");

	// Acquire wakelock, request UTC time and update status
	utils::Wakelock::acquire();
	requestUtcTime();
	statusUpdate(GnssStatusValue::SESSION_BEGIN);

	// Start the navigation
	startNavigation();

	return 0;
}

int AbstractDevice::stop()
{
	ALOGI("Stop navigation");

	// Stop the navigation
	stopNavigation();

	// Release wakelock and update status
	utils::Wakelock::release();
	statusUpdate(GnssStatusValue::SESSION_END);

	return 0;
}

void AbstractDevice::setTimestamp(GnssUtcTime t)
{
	timestamp = t;
	location->timestamp(t);
}

void AbstractDevice::emitNmea(const NmeaMessage & nmea)
{
	onNmea(timestamp, nmea);
}

Result<Version, ValueStatus>
	AbstractDevice::getProductVersion(const std::string & productName) const
{
	if(versions->size() == 0)
		return ValueStatus::NOT_AVAILABLE;

	if(versions->count(productName) == 0)
		return ValueStatus::NOT_AVAILABLE;

	return versions->at(productName);
}

void AbstractDevice::newVersionNumber(const model::Version & version)
{
	(*versions)[version.getProduct()] = version;
	onVersionNumber(version);
}

void AbstractDevice::sendMessageRequest(const model::Message & message)
{
	sendMessage(*this, message);
}

int AbstractDevice::getConstMask()
{
	return gnssConstMask;
}

void AbstractDevice::setConstMask(int mask)
{
	gnssConstMask = mask;
}

int AbstractDevice::setGNSSConstellationMask(int mask)
{
	int error = 0;
	int tmpMask = mask & ~4; // Remove QZSS before checking validity since QZSS is valid with all combinations

	//check if the new mask is already applied
	if(mask == gnssConstMask)
		return error;

	//check if the mask is a valid mask
	if(!((tmpMask == 1) || (tmpMask == 2) || (tmpMask == 3) || (tmpMask == 8) || (tmpMask == 9)
	 || (tmpMask == 10) || (tmpMask == 128) || (tmpMask == 129) || (tmpMask == 130)))
		return 1;
		
	//Send COLD start command to invalidate current almanac, ephemeris, position and time
	model::Message coldStartMsg;
	coldStartMsg.id = MessageId::ColdStart;
	coldStartMsg.parameters.push_back(utils::createFromString("15"));
	sendMessage(*this, coldStartMsg);

	//Suspend GNSS engine
	model::Message gpsSuspendMsg;
	gpsSuspendMsg.id = MessageId::GpsSuspend;
	sendMessage(*this, gpsSuspendMsg);

	//Clear the BD+GAL enable bits
	model::Message setParClearBDGALMsg;
	setParClearBDGALMsg.id = MessageId::SetPar;
	setParClearBDGALMsg.parameters.push_back(utils::createFromString("1227"));
	setParClearBDGALMsg.parameters.push_back(utils::createFromString("3C0"));
	setParClearBDGALMsg.parameters.push_back(utils::createFromString("2"));
	sendMessage(*this, setParClearBDGALMsg);

	//Clear the GPS+GLO enable bits
	model::Message setParClearGPSGLOMsg;
	setParClearGPSGLOMsg.id = MessageId::SetPar;
	setParClearGPSGLOMsg.parameters.push_back(utils::createFromString("1200"));
	setParClearGPSGLOMsg.parameters.push_back(utils::createFromString("630000"));
	setParClearGPSGLOMsg.parameters.push_back(utils::createFromString("2"));
	sendMessage(*this, setParClearGPSGLOMsg);

	switch(tmpMask)
	{
		case 1:		// GPS
		{
			//Enable GPS
			model::Message setParEnableGPSMsg;
			setParEnableGPSMsg.id = MessageId::SetPar;
			setParEnableGPSMsg.parameters.push_back(utils::createFromString("1200"));
			setParEnableGPSMsg.parameters.push_back(utils::createFromString("410000"));
			setParEnableGPSMsg.parameters.push_back(utils::createFromString("1"));
			sendMessage(*this, setParEnableGPSMsg);	
		}
		break;

		case 2:		// GLONASS
		{
			//Enable GLONASS
			model::Message setParEnableGLOMsg;
			setParEnableGLOMsg.id = MessageId::SetPar;
			setParEnableGLOMsg.parameters.push_back(utils::createFromString("1200"));
			setParEnableGLOMsg.parameters.push_back(utils::createFromString("220000"));
			setParEnableGLOMsg.parameters.push_back(utils::createFromString("1"));
			sendMessage(*this, setParEnableGLOMsg);
		}
		break;

		case 3:		// GPS+GLONASS
		{
			//Enable GPS+GLONASS
			model::Message setParEnableGPSGLOMsg;
			setParEnableGPSGLOMsg.id = MessageId::SetPar;
			setParEnableGPSGLOMsg.parameters.push_back(utils::createFromString("1200"));
			setParEnableGPSGLOMsg.parameters.push_back(utils::createFromString("630000"));
			setParEnableGPSGLOMsg.parameters.push_back(utils::createFromString("1"));
			sendMessage(*this, setParEnableGPSGLOMsg);
		}
		break;

		case 8:		// GALILEO
		{
			//Enable GALILEO
			model::Message setParEnableGALMsg;
			setParEnableGALMsg.id = MessageId::SetPar;
			setParEnableGALMsg.parameters.push_back(utils::createFromString("1227"));
			setParEnableGALMsg.parameters.push_back(utils::createFromString("0C0"));
			setParEnableGALMsg.parameters.push_back(utils::createFromString("1"));
			sendMessage(*this, setParEnableGALMsg);
		}
		break;

		case 9:		// GPS+GALILEO
		{
			//Enable GPS
			model::Message setParEnableGPSMsg;
			setParEnableGPSMsg.id = MessageId::SetPar;
			setParEnableGPSMsg.parameters.push_back(utils::createFromString("1200"));
			setParEnableGPSMsg.parameters.push_back(utils::createFromString("410000"));
			setParEnableGPSMsg.parameters.push_back(utils::createFromString("1"));
			sendMessage(*this, setParEnableGPSMsg);	

			//Enable GALILEO
			model::Message setParEnableGALMsg;
			setParEnableGALMsg.id = MessageId::SetPar;
			setParEnableGALMsg.parameters.push_back(utils::createFromString("1227"));
			setParEnableGALMsg.parameters.push_back(utils::createFromString("0C0"));
			setParEnableGALMsg.parameters.push_back(utils::createFromString("1"));
			sendMessage(*this, setParEnableGALMsg);
		}
		break;

		case 10:	// GLONASS+GALILEO
		{
			//Enable GLONASS
			model::Message setParEnableGLOMsg;
			setParEnableGLOMsg.id = MessageId::SetPar;
			setParEnableGLOMsg.parameters.push_back(utils::createFromString("1200"));
			setParEnableGLOMsg.parameters.push_back(utils::createFromString("220000"));
			setParEnableGLOMsg.parameters.push_back(utils::createFromString("1"));
			sendMessage(*this, setParEnableGLOMsg);

			//Enable GALILEO
			model::Message setParEnableGALMsg;
			setParEnableGALMsg.id = MessageId::SetPar;
			setParEnableGALMsg.parameters.push_back(utils::createFromString("1227"));
			setParEnableGALMsg.parameters.push_back(utils::createFromString("0C0"));
			setParEnableGALMsg.parameters.push_back(utils::createFromString("1"));
			sendMessage(*this, setParEnableGALMsg);
		}
		break;

		case 128:	// BEIDOU
		{
			//Enable BEIDOU
			model::Message setParEnableBDMsg;
			setParEnableBDMsg.id = MessageId::SetPar;
			setParEnableBDMsg.parameters.push_back(utils::createFromString("1227"));
			setParEnableBDMsg.parameters.push_back(utils::createFromString("300"));
			setParEnableBDMsg.parameters.push_back(utils::createFromString("1"));
			sendMessage(*this, setParEnableBDMsg);
		}
		break;

		case 129:	// GPS+BEIDOU
		{
			//Enable GPS
			model::Message setParEnableGPSMsg;
			setParEnableGPSMsg.id = MessageId::SetPar;
			setParEnableGPSMsg.parameters.push_back(utils::createFromString("1200"));
			setParEnableGPSMsg.parameters.push_back(utils::createFromString("410000"));
			setParEnableGPSMsg.parameters.push_back(utils::createFromString("1"));
			sendMessage(*this, setParEnableGPSMsg);			

			//Enable BEIDOU
			model::Message setParEnableBDMsg;
			setParEnableBDMsg.id = MessageId::SetPar;
			setParEnableBDMsg.parameters.push_back(utils::createFromString("1227"));
			setParEnableBDMsg.parameters.push_back(utils::createFromString("300"));
			setParEnableBDMsg.parameters.push_back(utils::createFromString("1"));
			sendMessage(*this, setParEnableBDMsg);
		}
		break;

		case 130:	// GALILEO+BEIDOU
		{
			//Enable GALILEO+BEIDOU
			model::Message setParEnableGALBDMsg;
			setParEnableGALBDMsg.id = MessageId::SetPar;
			setParEnableGALBDMsg.parameters.push_back(utils::createFromString("1227"));
			setParEnableGALBDMsg.parameters.push_back(utils::createFromString("3C0"));
			setParEnableGALBDMsg.parameters.push_back(utils::createFromString("1"));
			sendMessage(*this, setParEnableGALBDMsg);

			
		}
		break;

		default:
			break;
	}

	// send $PSTMSAVEPAR message
	model::Message saveParMsg;
	saveParMsg.id = MessageId::SavePar;
	sendMessage(*this, saveParMsg);

	// reset Teseo software
	model::Message systemResetMsg;
	systemResetMsg.id = MessageId::SystemReset;
	sendMessage(*this, systemResetMsg);

	return error;
}

int AbstractDevice::getGNSSConstellationMask()
{
	return getConstMask();
}

} // namespace device
} // namespace stm
