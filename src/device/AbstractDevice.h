/**
 * @file AbstractDevice.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_ABSTRACT_DEVICE_H
#define TESEO_HAL_ABSTRACT_DEVICE_H

#include <mutex>
#include <hardware/gps.h>

#include "../Signal.h"
#include "../model/NmeaMessage.h"
#include "../model/Location.h"
#include "../decoder/AbstractDecoder.h"
#include "../stream/IStream.h"
#include "../thread/Thread.h"

namespace stm {
namespace decoder {
	class NmeaDecoder;
	namespace nmea {
		struct decoders;
	} // namespace nmea
} // namespace decoder

namespace device {

/**
 * @brief      Abstract Device manager
 * 
 * @details    An abstract device store the Teseo current state in the HAL. This state is updated by
 * the attached decoder. 
 */
class AbstractDevice :
	public Trackable
{
private:
	GpsUtcTime timestamp;

	stream::IStream * stream;

	decoder::AbstractDecoder * decoder;

	Location location;

protected:

	// Allow NmeaDecoder to use emitNmea
	friend class stm::decoder::NmeaDecoder;

	// Allow decoding functions to update device data model
	friend struct stm::decoder::nmea::decoders;

	/**
	 * @brief      Abstract device constructor
	 * @details    The constructor is protected so it must be inherited to be instanciated.
	 */
	AbstractDevice();

	/**
	 * @brief      Connects the stream to the decoder.
	 */
	void connectStreamToDecoder();

	/**
	 * @brief      Sets the stream.
	 *
	 * @param      s     Pointer to a stream interface instance
	 */
	void setStream(stream::IStream * s);

	/**
	 * @brief      Sets the decoder.
	 *
	 * @param      d     Pointer to an abstract decoder instance
	 */
	void setDecoder(decoder::AbstractDecoder * d);

	/**
	 * @brief      Sets the timestamp.
	 *
	 * @param[in]  timestamp  The timestamp
	 */
	void setTimestamp(GpsUtcTime timestamp);

	/**
	 * @brief      Gets the location.
	 *
	 * @return     The location.
	 */
	Location & getLocation() { return location; }

	/**
	 * @brief      Emit a NMEA message
	 *
	 * @param[in]  nmea  The nmea message to emit
	 */
	void emitNmea(const NmeaMessage & nmea);

	/**
	 * @brief      Trigger device update
	 * @details    The device will emit signals to inform the HAL that its data has been updated.
	 */
	void update();

public:
	virtual ~AbstractDevice() { }

	GpsUtcTime getTimestamp() const { return timestamp; }

	/**
	 * @brief      Gets the location.
	 *
	 * @return     Const reference to the location.
	 */
	const Location & getLocation() const { return location; }

	/**
	 * @brief      Start the virtual device
	 *
	 * @return     0 on success, 1 on failure
	 */
	int start();

	/**
	 * @brief      Stop the virtual device
	 *
	 * @return     0 on success, 1 on failure
	 */
	int stop();

	/**
	 * NMEA signal
	 */
	Signal<void, GpsUtcTime, const NmeaMessage &> onNmea;

	/**
	 * Location update signal
	 */
	Signal<void, Location &> locationUpdate;
};

} // namespace device
} // namespace stm

#endif // TESEO_HAL_ABSTRACT_DEVICE_H