#ifndef TESEO_HAL_TESEO_H
#define TESEO_HAL_TESEO_H

extern "C" {

int teseo_hal_open_device(
    const struct hw_module_t * module,
    char const * name,
    struct hw_device_t ** dev);
}

#endif // TESEO_HAL_TESEO_H