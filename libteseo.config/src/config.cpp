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

    READ_VAL(stagps.enable, CFG_DEF_STAGPS_ENABLE);

    READ_VAL(stagps.predictive.enable,   CFG_DEF_STAGPS_PREDICTIVE_ENABLE);
    READ_VAL(stagps.predictive.host,     CFG_DEF_STAGPS_PREDICTIVE_HOST);
    READ_VAL(stagps.predictive.port,     CFG_DEF_STAGPS_PREDICTIVE_PORT);
    READ_VAL(stagps.predictive.modelId,  CFG_DEF_STAGPS_PREDICTIVE_MODEL_ID);
    READ_VAL(stagps.predictive.deviceId, CFG_DEF_STAGPS_PREDICTIVE_DEVICE_ID);
    READ_VAL(stagps.predictive.seedType, CFG_DEF_STAGPS_PREDICTIVE_SEED_TYPE);
    READ_VAL(stagps.predictive.basePath, CFG_DEF_STAGPS_PREDICTIVE_BASE_PATH);

    ALOGI("Done");

    return config;
}

const Configuration & get()
{
    return config;
}

} // namespace config
} // namespace stm