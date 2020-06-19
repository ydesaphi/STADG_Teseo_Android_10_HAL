/*
* This file is part of Teseo Android HAL
*
* Copyright (c) 2020, STMicroelectronics - All Rights Reserved
* Author(s): Muhamad Rahmady <muhamad.rahmady@st.com> for STMicroelectronics.
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
/**
 * @brief Header for Gnss v1.1
 * @file Gnss_1_1.h
 * @author Muhamad Rahmady <muhamad.rahmady@st.com>
 * @copyright 2020, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_GNSS_1_1_H
#define TESEO_HAL_GNSS_1_1_H

//#include <android/hardware/gnss/1.0/IAGnss.h>
//#include <android/hardware/gnss/1.0/IAGnssCallback.h>
//#include <android/hardware/gnss/1.0/IAGnssRil.h>
//#include <android/hardware/gnss/1.0/IAGnssRilCallback.h>
//#include <android/hardware/gnss/1.0/IGnssBatching.h>
//#include <android/hardware/gnss/1.0/IGnssBatchingCallback.h>
//#include <android/hardware/gnss/1.0/IGnssDebug.h>
//#include <android/hardware/gnss/1.0/IGnssGeofencing.h>
//#include <android/hardware/gnss/1.0/IGnssGeofenceCallback.h>
//#include <android/hardware/gnss/1.0/IGnssNavigationMessage.h>
//#include <android/hardware/gnss/1.0/IGnssNavigationMessageCallback.h>
//#include <android/hardware/gnss/1.0/IGnssNi.h>
//#include <android/hardware/gnss/1.0/IGnssNiCallback.h>
//#include <android/hardware/gnss/1.0/IGnssXtra.h>
//#include <android/hardware/gnss/1.0/IGnssXtraCallback.h>

#include <android/hardware/gnss/1.1/IGnss.h>
//#include <android/hardware/gnss/1.1/IGnssCallback.h>
//#include <android/hardware/gnss/1.1/IGnssConfiguration.h>
//#include <android/hardware/gnss/1.1/IGnssMeasurement.h>
//#include <android/hardware/gnss/1.1/IGnssMeasurementCallback.h>

using ::android::sp;

using ::android::hardware::gnss::V1_0::IAGnss;
using ::android::hardware::gnss::V1_0::IAGnssCallback;
using ::android::hardware::gnss::V1_0::IAGnssRil;
using ::android::hardware::gnss::V1_0::IAGnssRilCallback;
using ::android::hardware::gnss::V1_0::IGnssBatching;
using ::android::hardware::gnss::V1_0::IGnssBatchingCallback;
using ::android::hardware::gnss::V1_0::IGnssDebug;
using ::android::hardware::gnss::V1_0::IGnssGeofencing;
using ::android::hardware::gnss::V1_0::IGnssGeofenceCallback;
using ::android::hardware::gnss::V1_0::IGnssNavigationMessage;
using ::android::hardware::gnss::V1_0::IGnssNavigationMessageCallback;
using ::android::hardware::gnss::V1_0::IGnssNi;
using ::android::hardware::gnss::V1_0::IGnssNiCallback;
using ::android::hardware::gnss::V1_0::IGnssXtra;
using ::android::hardware::gnss::V1_0::IGnssXtraCallback;

using ::android::hardware::gnss::V1_1::IGnss;
using ::android::hardware::gnss::V1_1::IGnssCallback;
using ::android::hardware::gnss::V1_1::IGnssConfiguration;
using ::android::hardware::gnss::V1_1::IGnssMeasurement;
using ::android::hardware::gnss::V1_1::IGnssMeasurementCallback;

/* types */
using GnssMax               = ::android::hardware::gnss::V1_0::GnssMax;
using GnssConstellationType = ::android::hardware::gnss::V1_0::GnssConstellationType;
using GnssLocationFlags     = ::android::hardware::gnss::V1_0::GnssLocationFlags;
using GnssLocation          = ::android::hardware::gnss::V1_0::GnssLocation;
using GnssUtcTime           = ::android::hardware::gnss::V1_0::GnssUtcTime;

/* V1_0/IAGnss */
using ApnIpType = IAGnss::ApnIpType;

/* V1_0/IAGnssCallback */
using AGnssType         = IAGnssCallback::AGnssType;
using AGnssStatusValue  = IAGnssCallback::AGnssStatusValue;
using AGnssStatusIpV4   = IAGnssCallback::AGnssStatusIpV4;
using AGnssStatusIpV6   = IAGnssCallback::AGnssStatusIpV6;

/* V1_0/IAGnssRil */
using SetIDType                 = IAGnssRil::SetIDType;
using NetworkType               = IAGnssRil::NetworkType;
using AGnssRefLocationType      = IAGnssRil::AGnssRefLocationType;
using AGnssRefLocationCellID    = IAGnssRil::AGnssRefLocationCellID;
using AGnssRefLocation          = IAGnssRil::AGnssRefLocation;

/* V1_0/IAGnssRilCallback */
using SetID = IAGnssRilCallback::ID;

/* V1_0/IGnss */
using GnssPositionMode          = IGnss::GnssPositionMode;
using GnssPositionRecurrence    = IGnss::GnssPositionRecurrence;
using GnssAidingData            = IGnss::GnssAidingData;

/* V1_1/IGnssCallback */
using GnssCapabilities  = IGnssCallback::Capabilities;
using GnssStatusValue   = IGnssCallback::GnssStatusValue;
using GnssSvFlags       = IGnssCallback::GnssSvFlags;
using GnssSvInfo        = IGnssCallback::GnssSvInfo;
using GnssSvStatus      = IGnssCallback::GnssSvStatus;
using GnssSystemInfo    = IGnssCallback::GnssSystemInfo;

/* V1_0/IGnssBatching */
using BatchingFlag      = IGnssBatching::Flag;
using BatchingOptions   = IGnssBatching::Options;

/* V1_0/IGnssBatchingCallback */
//NA

/* V1_1/IGnssConfiguration */
using BlacklistedSource     = IGnssConfiguration::BlacklistedSource;

/* V1_0/IGnssConfiguration */
using SuplMode              = IGnssConfiguration::SuplMode;
using GpsLock               = IGnssConfiguration::GpsLock;
using LppProfile            = IGnssConfiguration::LppProfile;
using GlonassPosProtocol    = IGnssConfiguration::GlonassPosProtocol;

/* V1_0/IGnssDebug */
using SatelliteEphemerisType    = IGnssDebug::SatelliteEphemerisType;
using SatelliteEphemerisSource  = IGnssDebug::SatelliteEphemerisSource;
using SatelliteEphemerisHealth  = IGnssDebug::SatelliteEphemerisHealth;
using PositionDebug             = IGnssDebug::PositionDebug;
using TimeDebug                 = IGnssDebug::TimeDebug;
using SatelliteData             = IGnssDebug::SatelliteData;
using DebugData                 = IGnssDebug::DebugData;

/* V1_0/IGnssGeofencing */
//NA

/* V1_0/IGnssGeofenceCallback */
using GeofenceTransition    = IGnssGeofenceCallback::GeofenceTransition;
using GeofenceAvailability  = IGnssGeofenceCallback::GeofenceAvailability;
using GeofenceStatus        = IGnssGeofenceCallback::GeofenceStatus;

/* V1_0/IGnssMeasurement */
using GnssMeasurementStatus = IGnssMeasurement::GnssMeasurementStatus;

/* V1_1/IGnssMeasurementCallback */
using GnssAccumulatedDeltaRangeState    =
        IGnssMeasurementCallback::GnssAccumulatedDeltaRangeState;
using GnssMeasurement   = IGnssMeasurementCallback::GnssMeasurement;
using GnssData          = IGnssMeasurementCallback::GnssData;

/* V1_0/IGnssMeasurementCallback */
using GnssClockFlags                    =
        IGnssMeasurementCallback::GnssClockFlags;
using GnssMeasurementFlags              =
        IGnssMeasurementCallback::GnssMeasurementFlags;
using GnssMultipathIndicator            =
        IGnssMeasurementCallback::GnssMultipathIndicator;
using GnssMeasurementState              =
        IGnssMeasurementCallback::GnssMeasurementState;

using GnssClock         = IGnssMeasurementCallback::GnssClock;

/* V1_0/IGnssNavigationMessage */
using GnssNavigationMessageStatus   =
        IGnssNavigationMessage::GnssNavigationMessageStatus;

/* V1_0/IGnssNavigationMessageCallback */
using GnssNavigationMessageType =
        IGnssNavigationMessageCallback::GnssNavigationMessageType;
using NavigationMessageStatus   =
        IGnssNavigationMessageCallback::NavigationMessageStatus;
using GnssNavigationMessage     =
        IGnssNavigationMessageCallback::GnssNavigationMessage;

/* V1_0/IGnssNi */
//NA

/* V1_0/IGnssNiCallback */
using GnssNiType            = IGnssNiCallback::GnssNiType;
using GnssNiNotifyFlags     = IGnssNiCallback::GnssNiNotifyFlags;
using GnssUserResponseType  = IGnssNiCallback::GnssUserResponseType;
using GnssNiEncodingType    = IGnssNiCallback::GnssNiEncodingType;
using GnssNiNotification    = IGnssNiCallback::GnssNiNotification;

/* V1_0/IGnssXtraCallback */
//NA

#endif  // TESEO_HAL_GNSS_1_1_H
