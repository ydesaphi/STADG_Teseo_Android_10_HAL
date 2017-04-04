#include "NmeaDevice.h"

#define LOG_TAG "teseo_hal_NmeaDevice"
#include <cutils/log.h>

#include "../Signal.h"
#include "../decoder/NmeaDecoder.h"
#include "../stream/NmeaStream.h"

namespace stm {
namespace device {

NmeaDevice::NmeaDevice() :
	AbstractDevice()
{
	setStream(new stream::NmeaStream("/dev/ttyAMA2"));
	setDecoder(new decoder::NmeaDecoder(*this));
	connectStreamToDecoder();
}

} // namespace device
} // namespace stm