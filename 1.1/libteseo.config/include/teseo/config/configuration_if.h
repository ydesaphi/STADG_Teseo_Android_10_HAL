/*
 * This file is part of Teseo Android HAL
 *
 * Copyright (c) 2016-2020, STMicroelectronics - All Rights Reserved
 * Author(s): Fabrice Deruy <fabrice.deruy@st.com> for STMicroelectronics.
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

#ifndef TESEO_HAL_CONFIG_IF_H
#define TESEO_HAL_CONFIG_IF_H

#include <teseo/utils/Signal.h>
#include <teseo/utils/singleton.h>
#include <vector>
#include <string>
#include <map>
#include <tuple>

namespace stm {
namespace config {

using namespace std;

#define SUPL_VER "SUPL_VER"
#define GPS_LOCK "GPS_LOCK"
#define SUPL_ES  "SUPL_ES"
#define SUPL_MODE  "SUPL_MODE"
#define LPP_PROFILE  "LPP_PROFILE"
#define GLO_PROTOCOL  "A_GLONASS_POS_PROTOCOL_SELECT"
#define EMERGENCY_PDN  "USE_EMERGENCY_PDN_FOR_EMERGENCY_SUPL"

#define MAJOR_VERSION_SHIFT 16
#define MAJOR_VERSION_MASK 0xFF
#define MINOR_VERSION_SHIFT 8
#define MINOR_VERSION_MASK 0xFF

#define GET_MAJ_VERSION_NB(x) ((x>>MAJOR_VERSION_SHIFT)&MAJOR_VERSION_MASK)
#define GET_MIN_VERSION_NB(x) ((x>>MINOR_VERSION_SHIFT)&MINOR_VERSION_MASK)

static const vector<string> vStrings = {
        SUPL_VER,
        GPS_LOCK,
        SUPL_ES,
        SUPL_MODE,
        LPP_PROFILE,
        GLO_PROTOCOL,
        EMERGENCY_PDN};


/**
 * @brief      Configuration Interface class
 *
 * @details    This class is responsible of:
 * - Interface for passing GNSS configuration
 *   info from platform to HAL
 */
class Config_If:public Trackable,
                public Singleton<Config_If> {
friend class Singleton<Config_If>;

private:
    bool m_emergencySUPL; //True if emergency SUPL is to be enabled

    bool m_emergencySuplPDN; //Use emergency PDN if true and regular PDN if false.


    /** SUPL_MODE configuration parameter.
     * MSB = 0x01 	Mobile Station Based
     * MSA = 0x02 	Mobile Station Assisted
     */
    uint8_t m_SuplMode;

    /**GPS_LOCK configuration parameter
     * MO = 0x01 	Lock Mobile Originated GPS functionalitues.
     * NI = 0x02 	Lock Network initiated GPS functionalities.
    */
    uint8_t m_GpsLock;

    /** LTE Positioning Profile settings
     * USER_PLANE = 0x01 	Enable LTE Positioning Protocol user plane
     * CONTROL_PLANE = 0x02 	Enable LTE Positioning Protocol Control plane
     */
    uint8_t m_LppProfile;

    /** positioning protocol on A-Glonass system
     * RRC_CPLANE = 0x01 	Radio Resource Control(RRC)control-plane.
     * RRLP_CPLANE = 0x02 	Radio Resource Location user-plane.
     * LPP_UPLANE = 0x04 	LTE Positioning Protocol User plane
    */
    uint8_t m_GlonassPosProtocol;

    /**
     * SUPL version requested by carrier
     */
    uint32_t m_SuplVersion;



    // Value-Definitions of the different String values
    enum StringValue {
        E_NotDefined = 0,
        E_SuplVer,
        E_GpsLock,
        E_SuplEs,
        E_SuplMode,
        E_LppProfile,
        E_GloPosProtSel,
        E_UseEmerPDNSupl,
        E_End
    };


// Map to associate the strings with the enum values
 map<string, StringValue> m_mapStringValues = {
    { SUPL_VER , E_SuplVer },
    { GPS_LOCK, E_GpsLock },
    { SUPL_ES , E_SuplEs },
    { SUPL_MODE , E_SuplMode },
    { LPP_PROFILE , E_LppProfile },
    { GLO_PROTOCOL , E_GloPosProtSel },
    { EMERGENCY_PDN , E_UseEmerPDNSupl },
    {"",E_NotDefined }
};



public:
    Config_If();

    void configuration_update(const char* config_data, int32_t length);

    bool emergencySuplEnabled(void);
    bool useEmergencyPDN(void);
    uint8_t getSuplMode(void);
    uint8_t getGpsFuncLockMode(void);
    uint8_t getLTEPosProfile(void);
    uint8_t getGloPosProtocol(void);
    uint8_t getSuplMajorVersion(void);
    uint8_t getSuplMinorVersion(void);
    string getSuplVersionAsString(void);



};


} // namespace config
} // namespace stm

#endif
