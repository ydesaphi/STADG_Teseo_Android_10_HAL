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
 * @brief Header for Gnss v2.0
 * @file Gnss_2_0.h
 * @author Muhamad Rahmady <muhamad.rahmady@st.com>
 * @copyright 2020, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_GNSS_2_0_H
#define TESEO_HAL_GNSS_2_0_H

//#include <android/hardware/gnss/2.0/IAGnss.h>
//#include <android/hardware/gnss/2.0/IAGnssCallback.h>
//#include <android/hardware/gnss/2.0/IAGnssRil.h>
#include <android/hardware/gnss/2.0/IGnss.h>
//#include <android/hardware/gnss/2.0/IGnssCallback.h>
//#include <android/hardware/gnss/2.0/IGnssBatching.h>
//#include <android/hardware/gnss/2.0/IGnssBatchingCallback.h>
//#include <android/hardware/gnss/2.0/IGnssConfiguration.h>
//#include <android/hardware/gnss/2.0/IGnssDebug.h>
//#include <android/hardware/gnss/2.0/IGnssMeasurement.h>
//#include <android/hardware/gnss/2.0/IGnssMeasurementCallback.h>

//#include <android/hardware/gnss/1.0/IAGnssRilCallback.h>
//#include <android/hardware/gnss/1.0/IGnssGeofencing.h>
//#include <android/hardware/gnss/1.0/IGnssGeofenceCallback.h>
//#include <android/hardware/gnss/1.0/IGnssNavigationMessage.h>
//#include <android/hardware/gnss/1.0/IGnssNavigationMessageCallback.h>
//#include <android/hardware/gnss/1.0/IGnssNi.h>
//#include <android/hardware/gnss/1.0/IGnssNiCallback.h>
//#include <android/hardware/gnss/1.0/IGnssXtra.h>
//#include <android/hardware/gnss/1.0/IGnssXtraCallback.h>

using ::android::sp;

using ::android::hardware::gnss::V2_0::IAGnss;
using ::android::hardware::gnss::V2_0::IAGnssCallback;
using ::android::hardware::gnss::V2_0::IAGnssRil;
using ::android::hardware::gnss::V2_0::IGnss;
using ::android::hardware::gnss::V2_0::IGnssCallback;
using ::android::hardware::gnss::V2_0::IGnssBatching;
using ::android::hardware::gnss::V2_0::IGnssBatchingCallback;
using ::android::hardware::gnss::V2_0::IGnssConfiguration;
using ::android::hardware::gnss::V2_0::IGnssDebug;
using ::android::hardware::gnss::V2_0::IGnssMeasurement;
using ::android::hardware::gnss::V2_0::IGnssMeasurementCallback;

using ::android::hardware::gnss::V1_0::IAGnssRilCallback;
using ::android::hardware::gnss::V1_0::IGnssGeofencing;
using ::android::hardware::gnss::V1_0::IGnssGeofenceCallback;
using ::android::hardware::gnss::V1_0::IGnssNavigationMessage;
using ::android::hardware::gnss::V1_0::IGnssNavigationMessageCallback;
using ::android::hardware::gnss::V1_0::IGnssNi;
using ::android::hardware::gnss::V1_0::IGnssNiCallback;
using ::android::hardware::gnss::V1_0::IGnssXtra;
using ::android::hardware::gnss::V1_0::IGnssXtraCallback;

/* V2_0/types */
using ElapsedRealtimeFlags  = ::android::hardware::gnss::V2_0::ElapsedRealtimeFlags;
using ElapsedRealtime       = ::android::hardware::gnss::V2_0::ElapsedRealtime;
using GnssLocation_2_0      = ::android::hardware::gnss::V2_0::GnssLocation;
//using GnssConstellationType_2_0 = ::android::hardware::gnss::V2_0::GnssConstellationType;
using GnssConstellationType = ::android::hardware::gnss::V2_0::GnssConstellationType;

/* V1_0/types */
using GnssMax               = ::android::hardware::gnss::V1_0::GnssMax;
//using GnssConstellationType = ::android::hardware::gnss::V1_0::GnssConstellationType;
using GnssLocationFlags     = ::android::hardware::gnss::V1_0::GnssLocationFlags;
using GnssLocation          = ::android::hardware::gnss::V1_0::GnssLocation;
using GnssUtcTime           = ::android::hardware::gnss::V1_0::GnssUtcTime;

/* V2_0/IAGnss */
using ApnIpType = IAGnss::ApnIpType;

/* V2_0/IAGnssCallback */
using AGnssType         = IAGnssCallback::AGnssType;
using AGnssStatusValue  = IAGnssCallback::AGnssStatusValue;

/* V2_0/IAGnssRil */
using NetworkCapability = ::android::hardware::gnss::V2_0::IAGnssRil::NetworkCapability;
using NetworkAttributes = ::android::hardware::gnss::V2_0::IAGnssRil::NetworkAttributes;

/* V1_0/IAGnssRil */
using SetIDType                 = ::android::hardware::gnss::V1_0::IAGnssRil::SetIDType;
using NetworkType               = ::android::hardware::gnss::V1_0::IAGnssRil::NetworkType;
using AGnssRefLocationType      = ::android::hardware::gnss::V1_0::IAGnssRil::AGnssRefLocationType;
using AGnssRefLocationCellID    = ::android::hardware::gnss::V1_0::IAGnssRil::AGnssRefLocationCellID;
using AGnssRefLocation          = ::android::hardware::gnss::V1_0::IAGnssRil::AGnssRefLocation;

/* V1_0/IAGnssRilCallback */
using SetID = IAGnssRilCallback::ID;

/* V1_0/IGnss */
using GnssPositionMode          = IGnss::GnssPositionMode;
using GnssPositionRecurrence    = IGnss::GnssPositionRecurrence;
using GnssAidingData            = IGnss::GnssAidingData;

/* V2_0/IGnssCallback */
//using GnssCapabilities_2_0  = ::android::hardware::gnss::V2_0::IGnssCallback::Capabilities;
using GnssCapabilities  = ::android::hardware::gnss::V2_0::IGnssCallback::Capabilities;
using GnssSvInfo_2_0    = ::android::hardware::gnss::V2_0::IGnssCallback::GnssSvInfo;

/* V1_0/IGnssCallback */
//using GnssCapabilities  = ::android::hardware::gnss::V1_0::IGnssCallback::Capabilities;
using GnssStatusValue   = ::android::hardware::gnss::V1_0::IGnssCallback::GnssStatusValue;
using GnssSvFlags       = ::android::hardware::gnss::V1_0::IGnssCallback::GnssSvFlags;
using GnssSvInfo        = ::android::hardware::gnss::V1_0::IGnssCallback::GnssSvInfo;
using GnssSvStatus      = ::android::hardware::gnss::V1_0::IGnssCallback::GnssSvStatus;
using GnssSystemInfo    = ::android::hardware::gnss::V1_0::IGnssCallback::GnssSystemInfo;

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

/* V2_0/IGnssDebug */
using SatelliteData_2_0     = ::android::hardware::gnss::V2_0::IGnssDebug::SatelliteData;
using DebugData_2_0         = ::android::hardware::gnss::V2_0::IGnssDebug::DebugData;

/* V1_0/IGnssDebug */
using SatelliteEphemerisType    = ::android::hardware::gnss::V1_0::IGnssDebug::SatelliteEphemerisType;
using SatelliteEphemerisSource  = ::android::hardware::gnss::V1_0::IGnssDebug::SatelliteEphemerisSource;
using SatelliteEphemerisHealth  = ::android::hardware::gnss::V1_0::IGnssDebug::SatelliteEphemerisHealth;
using PositionDebug             = ::android::hardware::gnss::V1_0::IGnssDebug::PositionDebug;
using TimeDebug                 = ::android::hardware::gnss::V1_0::IGnssDebug::TimeDebug;
using SatelliteData             = ::android::hardware::gnss::V1_0::IGnssDebug::SatelliteData;
using DebugData                 = ::android::hardware::gnss::V1_0::IGnssDebug::DebugData;

/* V1_0/IGnssGeofencing */
//NA

/* V1_0/IGnssGeofenceCallback */
using GeofenceTransition    = IGnssGeofenceCallback::GeofenceTransition;
using GeofenceAvailability  = IGnssGeofenceCallback::GeofenceAvailability;
using GeofenceStatus        = IGnssGeofenceCallback::GeofenceStatus;

/* V1_0/IGnssMeasurement */
using GnssMeasurementStatus = IGnssMeasurement::GnssMeasurementStatus;

/* V2_0/IGnssMeasurementCallback */
//using GnssMeasurementState_2_0  =
using GnssMeasurementState  =
        ::android::hardware::gnss::V2_0::IGnssMeasurementCallback::GnssMeasurementState;
//using GnssMeasurement_2_0   = ::android::hardware::gnss::V2_0::IGnssMeasurementCallback::GnssMeasurement;
using GnssMeasurement   = ::android::hardware::gnss::V2_0::IGnssMeasurementCallback::GnssMeasurement;
//using GnssData_2_0          = ::android::hardware::gnss::V2_0::IGnssMeasurementCallback::GnssData;
using GnssData          = ::android::hardware::gnss::V2_0::IGnssMeasurementCallback::GnssData;

/* V1_1/IGnssMeasurementCallback */
using GnssAccumulatedDeltaRangeState    =
        ::android::hardware::gnss::V1_1::IGnssMeasurementCallback::GnssAccumulatedDeltaRangeState;
//using GnssMeasurement   = ::android::hardware::gnss::V1_1::IGnssMeasurementCallback::GnssMeasurement;
//using GnssData          = ::android::hardware::gnss::V1_1::IGnssMeasurementCallback::GnssData;

/* V1_0/IGnssMeasurementCallback */
using GnssClockFlags                    =
        ::android::hardware::gnss::V1_0::IGnssMeasurementCallback::GnssClockFlags;
using GnssMeasurementFlags              =
        ::android::hardware::gnss::V1_0::IGnssMeasurementCallback::GnssMeasurementFlags;
using GnssMultipathIndicator            =
        ::android::hardware::gnss::V1_0::IGnssMeasurementCallback::GnssMultipathIndicator;
//using GnssMeasurementState              =
//        ::android::hardware::gnss::V1_0::IGnssMeasurementCallback::GnssMeasurementState;

using GnssClock         = ::android::hardware::gnss::V1_0::IGnssMeasurementCallback::GnssClock;

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

#endif  // TESEO_HAL_GNSS_2_0_H
