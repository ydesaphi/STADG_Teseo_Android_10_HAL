/*
* This file is part of Teseo Android HAL
*
* Copyright (c) 2016-2018, STMicroelectronics - All Rights Reserved
* Author(s): Fabrice Deruy <fabrice.deruy@st.com> for STMicroelectronics.
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

#include <teseo/config/configuration_if.h>
#include <sstream>

#define LOG_TAG "teseo_hal_ConfigIf"
#include <log/log.h>


namespace stm {
namespace config {

class findStringAndValue {
public:
    tuple<string,string> operator()(string s) {
        string name = "";
        string value = "";
        for (auto&& ss : vStrings)
        {
            std::size_t found = s.find(ss);
            if (found != std::string::npos)
            {        
                name = s.substr(found, ss.length());
            }
        }
        std::size_t found = s.find("=");
        if (found != std::string::npos)
        {
            value = s.substr(found + 1);
        }

        return make_tuple(name, value);
    }
};



Config_If::Config_If():
    m_emergencySUPL(false),
    m_emergencySuplPDN(false),
    m_SuplMode(1),
    m_GpsLock(0),
    m_LppProfile(0),
    m_GlonassPosProtocol(2),
    m_SuplVersion(0)
{

}

void Config_If::configuration_update(const char* config_data, int32_t length){
    (void)(length);
    
    findStringAndValue myFunc;
    string var_name;
    string var_value;

    tie(var_name, var_value) = myFunc(string(config_data));

    switch (m_mapStringValues[var_name])
    {
    case E_SuplVer:
        m_SuplVersion = atoi(var_value.c_str());
        ALOGI("Set SUPL version: %s", getSuplVersionAsString().c_str());
        break;
    case E_SuplMode:
        m_SuplMode = (uint8_t)(atoi(var_value.c_str()));
        ALOGI("Set SUPL mode: %d", m_SuplMode);
        break;
    case E_GpsLock:
        m_GpsLock = (uint8_t)(atoi(var_value.c_str()));
        ALOGI("Set GPS lock: %d", m_GpsLock);
        break;
    case E_SuplEs:
        m_emergencySUPL = (bool)(atoi(var_value.c_str()));
        ALOGI("Emergency SUPL is %s", m_emergencySUPL?"enabled":"disabled");
        break;
    case E_LppProfile:
        m_LppProfile = (uint8_t)(atoi(var_value.c_str()));
        ALOGI("Set LPP profile: %d", m_LppProfile);
	break;
    case E_GloPosProtSel:
        m_GlonassPosProtocol = (uint8_t)(atoi(var_value.c_str()));
        ALOGI("A-Glonass system positioning protocol: %d", m_GlonassPosProtocol);
        break;
    case E_UseEmerPDNSupl:
         m_emergencySuplPDN = (bool)(atoi(var_value.c_str()));
        ALOGI("Emergency PDN is %s", m_emergencySuplPDN?"selected":"not selected");
        break;
    case E_NotDefined:
    default:
        ALOGE("Input data not defined");
        break;
    }
}

bool Config_If::emergencySuplEnabled(void){
    return m_emergencySUPL;
}
bool Config_If::useEmergencyPDN(void){
    return m_emergencySuplPDN;
}
uint8_t Config_If::getSuplMode(void){
    return m_SuplMode;
}
uint8_t Config_If::getGpsFuncLockMode(void)
{
    return m_GpsLock;
}
uint8_t Config_If::getLTEPosProfile(void)
{
    return m_LppProfile;
}
uint8_t Config_If::getGloPosProtocol(void)
{
    return m_GlonassPosProtocol;
}

uint8_t Config_If::getSuplMajorVersion(void)
{
    return GET_MAJ_VERSION_NB(m_SuplVersion);
}
uint8_t Config_If::getSuplMinorVersion(void)
{
    return GET_MIN_VERSION_NB(m_SuplVersion); 
}

string Config_If::getSuplVersionAsString(void)
{
    ostringstream flux;
    flux << GET_MAJ_VERSION_NB(m_SuplVersion) << "." << GET_MIN_VERSION_NB(m_SuplVersion);
    return flux.str(); 
}


} // namespace config
} // namespace stm
