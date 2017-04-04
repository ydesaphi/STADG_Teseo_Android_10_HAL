#include "HalManager.h"

#define LOG_TAG "teseo_hal_HalManager"
#include <cutils/log.h>

#include "utils/Time.h"
#include "device/NmeaDevice.h"
#include "LocServiceProxy.h"

namespace stm {

HalManager HalManager::instance;

HalManager::HalManager() :
	setCapabilites("HalManager::setCapabilites")
{
	ALOGI("Create HAL manager");

	device = nullptr;

	setCapabilites.connect(SlotFactory::create(&(LocServiceProxy::gps::sendCapabilities)));
}

HalManager::~HalManager()
{ }

HalManager & HalManager::getInstance()
{
	return instance;
}

int HalManager::init(GpsCallbacks * cb)
{
	(void)(cb);

	ALOGI("Initialize the HAL");
	device = new device::NmeaDevice();

	ALOGI("Set capabilities");
	setCapabilites(GPS_CAPABILITY_SCHEDULING     |
	               GPS_CAPABILITY_SINGLE_SHOT    |
	               GPS_CAPABILITY_ON_DEMAND_TIME |
	               GPS_CAPABILITY_MEASUREMENTS   |
	               GPS_CAPABILITY_NAV_MESSAGES);

	LocServiceProxy::gps::getSignals().injectTime.connect(
		SlotFactory::create(utils::injectTime));

	return 0;
}

void HalManager::cleanup(void)
{
	delete device;
}

} // namespace stm
