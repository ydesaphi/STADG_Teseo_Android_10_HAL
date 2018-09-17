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
#ifndef TESEO_HAL_MODEL_SAT_INFO_H
#define TESEO_HAL_MODEL_SAT_INFO_H

#include <functional>

#include "Constellations.h"

namespace stm {

enum class SatInfoFlags : uint8_t {
	None         = GNSS_SV_FLAGS_NONE,
	HasAlmanac   = GNSS_SV_FLAGS_HAS_ALMANAC_DATA,
	HasEphemeris = GNSS_SV_FLAGS_HAS_EPHEMERIS_DATA,
	UsedInFix    = GNSS_SV_FLAGS_USED_IN_FIX
};

Constellation prn2constellation(int16_t prn);
int16_t prn2svid(Constellation constellation, int16_t prn);

class SatIdentifier {
private:
	int16_t prn;
	int16_t svid;
	Constellation constellation;

public:
	SatIdentifier();

	explicit SatIdentifier(int16_t prn);

	SatIdentifier(Constellation constellation, int16_t svid);

	SatIdentifier(const SatIdentifier & other);

	int16_t getPrn() const
	{ return prn; }

	int16_t getSvid() const
	{ return svid; }

	Constellation getConstellation() const
	{ return constellation; }

	SatIdentifier & operator = (const SatIdentifier & other);

	bool operator == (const SatIdentifier & other) const;

	bool operator != (const SatIdentifier & other) const;

	bool operator < (const SatIdentifier & other) const;

	bool operator > (const SatIdentifier & other) const;

	bool operator <= (const SatIdentifier & other) const;

	bool operator >= (const SatIdentifier & other) const;
};

class SatInfo {
private:
	/**
	 * Satellite identifier: PRN or SVID and constellation
	 */
	SatIdentifier id;

	/**
	 * Elevation in degrees, max 90°
	 */
	float elevation;

	/**
	 * Azimuth in degrees, [0, 359]
	 */
	float azimuth;

	/**
	 * SNR (C/No) [0, 99] dB-Hz, -1 when not tracking
	 */
	float snr;

	/**
	 * Satellite is tracked
	 */
	bool tracked;

	/**
	 * Satellite has ephemeris
	 */
	bool ephemeris;

	/**
	 * Satellite has almanac
	 */
	bool almanac;

	/**
	 * Satellite is used in fix
	 */
	bool usedInFix;

public:
	explicit SatInfo(const SatIdentifier & id = SatIdentifier());

	SatInfo(const SatIdentifier & id,
		float elevation,
		float azimuth,
		float snr,
		bool tracked = true,
		bool hasEphemeris = true,
		bool hasAlmanac = true,
		bool usedInFix = true);

	SatInfo(const SatInfo & other);

	const SatIdentifier & getId() const;

	float getElevation() const;

	float getAzimuth() const;

	float getSnr() const;

	bool isTracked() const;

	bool hasEphemeris() const;

	bool hasAlmanac() const;

	bool isUsedInFix() const;

	SatInfo & setElevation(float elevation);

	SatInfo & setAzimuth(float azimuth);

	SatInfo & setSnr(float snr);

	SatInfo & setTracked(bool tracked);

	SatInfo & setEphemeris(bool ephemeris);

	SatInfo & setAlmanac(bool almanac);

	SatInfo & setUsedInFix(bool usedInFix);

	void copyToGnssSvInfo(GnssSvInfo * dest) const;
};

} // namespace stm

namespace std {

template <>
struct hash<stm::SatIdentifier> {
	typedef stm::SatIdentifier argument_type;
	typedef std::size_t result_type;

	result_type operator()(argument_type const & id) const
	{
		return static_cast<result_type>(id.getPrn());
	}
};

} // namespace std

#endif // TESEO_HAL_MODEL_SAT_INFO_H