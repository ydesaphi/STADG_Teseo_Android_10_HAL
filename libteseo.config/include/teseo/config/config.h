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
    } device;

    /**
     * ST-AGPS Configuration
     */
    struct Stagps {

        bool enable; ///< Flag used to enable or disable ST-AGPS globally

        /**
         * Predictive configuration
         */
        struct Predictive {
            bool enable;          ///< Enable or disable predictive
            std::string host;     ///< Seed server host
            int port;             ///< Seed server port
            std::string basePath; ///< Seed server base path
            std::string modelId;  ///< Model identifier
            std::string deviceId; ///< Device identifier
            int seedType;         ///< Seed type
        } predictive;

    } stagps;

};

const Configuration & read(const std::string & path = std::string("/etc/gps.conf"));

const Configuration & get();

} // namespace config
} // namespace stm

#endif // TESEO_HAL_CONFIG_H