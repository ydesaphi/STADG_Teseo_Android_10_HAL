#include <teseo/teseo.h>
#include <teseo/LocServiceProxy.h>

extern "C" {

int teseo_hal_open_device(
    const struct hw_module_t * module,
    char const * name,
    struct hw_device_t ** dev)
{
    return stm::LocServiceProxy::openDevice(module, name, dev);
}

}