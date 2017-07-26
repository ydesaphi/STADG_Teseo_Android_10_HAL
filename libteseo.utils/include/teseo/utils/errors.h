/**
 * @brief Error handlers
 * @file errors.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_ERRORS_H
#define TESEO_HAL_ERRORS_H

#include <errno.h>

#define CHECK_ERROR(value, error_descriptor_func, ...) { \
if(value == -1) { error_descriptor_func(errno); } \
else            { ALOGV(__VA_ARGS__); } }

namespace stm {
namespace errors {

int read(int err);

int open(int err);

int close(int err);

int pipe(int err);

int write(int err);

} // namespace errors
} // namespace stm

#endif