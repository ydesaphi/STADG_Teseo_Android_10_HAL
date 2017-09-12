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
 * @file config.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/config/config.h>

#define LOG_TAG "teseo_hal_config"
#include <cutils/log.h>
#include <memory>
#include <string>

#include <teseo/vendor/cpptoml.h>

#include "defaultconfig.h"

using namespace std;

namespace stm {
namespace config {

shared_ptr<cpptoml::table> rawConfig;

Configuration config;

template <typename T>
T get_or_default(cpptoml::option<T> opt, const T & defaultValue)
{
    return opt ? *opt : defaultValue;
}

#define READ_VAL_EXPL(key, type, def) \
    config.key = get_or_default(cfg.get_qualified_as<type>(#key), def)

#define READ_VAL(key, def) \
    config.key = get_or_default(cfg.get_qualified_as<decltype(def)>(#key), def)

const Configuration & read(const string & path)
{
    ALOGI("Parse configuration file: %s", path.c_str());
    rawConfig = cpptoml::parse_file(path);

    ALOGI("Dereference configuration object");
    const auto & cfg = *rawConfig;

    ALOGI("Read configuration");
    READ_VAL(device.tty, CFG_DEF_DEVICE_TTY);
    READ_VAL(device.speed, CFG_DEF_DEVICE_SPEED);

    READ_VAL(constellations.gps,     CFG_DEF_CONSTELLATIONS_GPS);
    READ_VAL(constellations.glonass, CFG_DEF_CONSTELLATIONS_GLONASS);
    READ_VAL(constellations.beidou,  CFG_DEF_CONSTELLATIONS_BEIDOU);
    READ_VAL(constellations.galileo, CFG_DEF_CONSTELLATIONS_GALILEO);

    READ_VAL(stagps.enable, CFG_DEF_STAGPS_ENABLE);

    READ_VAL(stagps.predictive.enable,    CFG_DEF_STAGPS_PREDICTIVE_ENABLE);
    READ_VAL(stagps.predictive.host,      CFG_DEF_STAGPS_PREDICTIVE_HOST);
    READ_VAL(stagps.predictive.port,      CFG_DEF_STAGPS_PREDICTIVE_PORT);
    READ_VAL(stagps.predictive.vendor_id, CFG_DEF_STAGPS_PREDICTIVE_VENDOR_ID);
    READ_VAL(stagps.predictive.model_id,  CFG_DEF_STAGPS_PREDICTIVE_MODEL_ID);
    READ_VAL(stagps.predictive.device_id, CFG_DEF_STAGPS_PREDICTIVE_DEVICE_ID);
    READ_VAL(stagps.predictive.seed_type, CFG_DEF_STAGPS_PREDICTIVE_SEED_TYPE);
    READ_VAL(stagps.predictive.base_path, CFG_DEF_STAGPS_PREDICTIVE_BASE_PATH);

    READ_VAL(stagps.realtime.enable,    CFG_DEF_STAGPS_REALTIME_ENABLE);
    READ_VAL(stagps.realtime.host,      CFG_DEF_STAGPS_REALTIME_HOST);
    READ_VAL(stagps.realtime.port,      CFG_DEF_STAGPS_REALTIME_PORT);
    READ_VAL(stagps.realtime.vendor_id, CFG_DEF_STAGPS_REALTIME_VENDOR_ID);
    READ_VAL(stagps.realtime.model_id,  CFG_DEF_STAGPS_REALTIME_MODEL_ID);
    READ_VAL(stagps.realtime.device_id, CFG_DEF_STAGPS_REALTIME_DEVICE_ID);
    READ_VAL(stagps.realtime.base_path, CFG_DEF_STAGPS_REALTIME_BASE_PATH);

    ALOGI("Done");

    return config;
}

const Configuration & get()
{
    return config;
}

} // namespace config
} // namespace stm