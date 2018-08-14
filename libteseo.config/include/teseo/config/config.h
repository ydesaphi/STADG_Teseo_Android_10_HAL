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
/**
 * @file config.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_CONFIG_H
#define TESEO_HAL_CONFIG_H

#include <iostream>

namespace stm {
namespace config {

/**
 * Configuration structure
 *
 * @details Configuration is parsed from /etc/gps.conf file
 * Each configuration block configures one component or sub-component of the HAL.
 */
struct Configuration {

    /**
     * Teseo device configuration
     */
    struct Device {
        std::string tty; ///< TTY connected to Teseo
        unsigned int speed; ///< Serial port baudrate
    } device;

    /**
     * Constellations supports
     */
    struct Constellations {
        bool gps;
        bool glonass;
        bool beidou;
        bool galileo;
    } constellations;

    /**
     * Assistance
     */
    struct Agnss
    {
        bool enable; ///< Flag used to enable or disable assistance including ST-AGPS
    } agnss;
    /**
     * ST-AGPS Configuration
     */
    struct Stagps {

        bool enable; ///< Flag used to enable or disable ST-AGPS globally

        /**
         * Predictive configuration
         */
        struct Predictive {
            bool enable;           ///< Enable or disable predictive
            std::string host;      ///< Seed server host
            int port;              ///< Seed server port
            std::string base_path; ///< Seed server base path
            std::string vendor_id; ///< Vendor identifier
            std::string model_id;  ///< Model identifier
            std::string device_id; ///< Device identifier
            int seed_type;         ///< Seed type
        } predictive;

        struct RealTime {
            bool enable;           ///< Enable or disable realtime
            std::string host;      ///< Seed server host
            int port;              ///< Seed server port
            std::string base_path; ///< Seed server base path
            std::string vendor_id; ///< Vendor identifier
            std::string model_id;  ///< Model identifier
            std::string device_id; ///< Device identifier
        } realtime;

    } stagps;

};

const Configuration & read(const std::string & path = std::string("/etc/gps.conf"));

const Configuration & get();

} // namespace config
} // namespace stm

#endif // TESEO_HAL_CONFIG_H