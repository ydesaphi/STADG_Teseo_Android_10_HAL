/**
 * @file FixQuality.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_MODEL_FIX_QUALITY
#define TESEO_HAL_MODEL_FIX_QUALITY

namespace stm {

/**
 * @brief      Fix quality enumeration
 */
enum class FixQuality : uint8_t
{
	Invalid           = 0, ///< Invalid Fix or not available
	GPS               = 1, ///< GPS Fix
	DGPS              = 2, ///< Differential GPS Fix
	PPS               = 3, ///< PPS Fix
	RealTimeKinematic = 4, ///< Real Time Kinematic
	FloatRTK          = 5, ///< Float RTK
	DeadReckoning     = 6, ///< Estimated (Dead-Reckoning)
	ManualInputMode   = 7, ///< Manual Input Mode
	SimulationMode    = 8, ///< Simulation Mode
};

/**
 * @brief      Convert integer to fix quality
 *
 * @param[in]  qual  The fix quality integer value
 *
 * @return     The fix quality enumeration value
 */
FixQuality FixQualityFromInt(uint8_t qual)
{
	if(qual < 0 || 8 < qual)
		return FixQuality::Invalid;

	return static_cast<FixQuality>(qual);
}

} // namespace stm

#endif // TESEO_HAL_MODEL_FIX_QUALITY