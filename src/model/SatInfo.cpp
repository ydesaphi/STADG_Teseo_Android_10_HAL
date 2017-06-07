#include "SatInfo.h"

#define LOG_TAG "teseo_hal_SatInfo"
#include <cutils/log.h>

#include <cstring>

#include <hardware/gps.h>

namespace stm {

SatInfo::SatInfo(const SatIdentifier & id) :
	id(id),
	elevation(-1),
	azimuth(-1),
	snr(-1),
	tracked(false),
	ephemeris(false),
	almanac(false),
	usedInFix(false)
{
	this->updateSvInfoStruct();
}

SatInfo::SatInfo(const SatIdentifier & id,
	float elevation,
	float azimuth,
	float snr,
	bool tracked,
	bool hasEphemeris,
	bool hasAlmanac,
	bool usedInFix) :
	id(id),
	elevation(elevation),
	azimuth(azimuth),
	snr(snr),
	tracked(tracked),
	ephemeris(hasEphemeris),
	almanac(hasAlmanac),
	usedInFix(usedInFix)
{
	this->updateSvInfoStruct();
}

SatInfo::SatInfo(const SatInfo & other) :
	id(other.id),
	elevation(other.elevation),
	azimuth(other.azimuth),
	snr(other.snr),
	tracked(other.tracked),
	ephemeris(other.ephemeris),
	almanac(other.almanac),
	usedInFix(other.usedInFix)
{
	this->updateSvInfoStruct();
}


const SatIdentifier & SatInfo::getId() const
{
	return this->id;
}

float SatInfo::getElevation() const
{
	return this->elevation;
}

float SatInfo::getAzimuth() const
{
	return this->azimuth;
}

float SatInfo::getSnr() const
{
	return this->snr;
}

bool SatInfo::isTracked() const
{
	return this->tracked;
}

bool SatInfo::hasEphemeris() const
{
	return this->ephemeris;
}

bool SatInfo::hasAlmanac() const
{
	return this->almanac;
}

bool SatInfo::isUsedInFix() const
{
	return this->usedInFix;
}

SatInfo & SatInfo::setElevation(float elevation)
{
	if(elevation > 90.)
	{
		ALOGE("SatInfo: elevation is out of range, %f > 90 deg.", elevation);
	}
	else
	{
		this->elevation = elevation;
		this->svInfoStruct.elevation = elevation;
	}

	return *this;
}

SatInfo & SatInfo::setAzimuth(float azimuth)
{
	if(azimuth < 0 || 359 < azimuth)
	{
		ALOGE("SatInfo: azimuth %f is out of range [0, 359] deg.", azimuth);
	}
	else
	{
		this->azimuth = azimuth;
		this->svInfoStruct.azimuth = azimuth;
	}

	return *this;
}

SatInfo & SatInfo::setSnr(float snr)
{
	if(snr < 0 || 99 < snr)
	{
		ALOGE("SatInfo: snr %f is out of range [0, 99] dB-Hz.", snr);
	}
	else
	{
		this->snr = snr;
		this->svInfoStruct.c_n0_dbhz = snr;
	}

	return *this;
}

SatInfo & SatInfo::setTracked(bool tracked)
{
	this->tracked = tracked;

	if(!tracked)
	{
		this->snr = -1;
		this->svInfoStruct.c_n0_dbhz = -1;
		this->setUsedInFix(false);
	}

	return *this;
}

SatInfo & SatInfo::setEphemeris(bool ephemeris)
{
	this->ephemeris = ephemeris;

	if(ephemeris)
		this->svInfoStruct.flags |= GNSS_SV_FLAGS_HAS_EPHEMERIS_DATA;
	else
		this->svInfoStruct.flags &= ~GNSS_SV_FLAGS_HAS_EPHEMERIS_DATA;

	return *this;
}

SatInfo & SatInfo::setAlmanac(bool almanac)
{
	this->almanac = almanac;

	if(almanac)
		this->svInfoStruct.flags |= GNSS_SV_FLAGS_HAS_ALMANAC_DATA;
	else
		this->svInfoStruct.flags &= ~GNSS_SV_FLAGS_HAS_ALMANAC_DATA;

	return *this;
}

SatInfo & SatInfo::setUsedInFix(bool usedInFix)
{
	this->usedInFix = usedInFix;

	if(usedInFix)
		this->svInfoStruct.flags |= GNSS_SV_FLAGS_USED_IN_FIX;
	else
		this->svInfoStruct.flags &= ~GNSS_SV_FLAGS_USED_IN_FIX;

	return *this;
}

const GnssSvInfo & SatInfo::toGnssSvInfo() const
{
	return this->svInfoStruct;
}

void SatInfo::copyToGnssSvInfo(GnssSvInfo * dest) const
{
	std::memcpy(dest, &this->svInfoStruct, sizeof(GnssSvInfo));
}

void SatInfo::updateSvInfoStruct()
{
	GnssSvInfo & svi = this->svInfoStruct;

	svi.size = sizeof(GnssSvInfo);
	svi.svid = this->id.getSvid();
	svi.constellation = ConstellationUtils::toAndroidType(this->id.getConstellation());
	svi.c_n0_dbhz = this->snr;
	svi.elevation = this->elevation;
	svi.azimuth = this->azimuth;
	svi.flags = (this->almanac   ? GNSS_SV_FLAGS_HAS_ALMANAC_DATA   : 0x00) |
	            (this->ephemeris ? GNSS_SV_FLAGS_HAS_EPHEMERIS_DATA : 0x00) |
	            (this->usedInFix ? GNSS_SV_FLAGS_USED_IN_FIX        : 0x00);
}

constexpr int16_t PRN_GPS_MIN = 1,
                  PRN_GPS_MAX = 32,
                  PRN_GLO_MIN = 65,
                  PRN_GLO_MAX = 98,
                  PRN_BEI_MIN = 141,
                  PRN_BEI_MAX = 170,
                  PRN_QZS_MIN = 193,
                  PRN_QZS_MAX = 200,
                  PRN_GAL_MIN = 301,
                  PRN_GAL_MAX = 330;

static Constellation prn2constellation(int16_t prn)
{
	if(PRN_GPS_MIN <= prn && prn <= PRN_GPS_MAX) return Constellation::Gps;
	if(PRN_GLO_MIN <= prn && prn <= PRN_GLO_MAX) return Constellation::Glonass;
	if(PRN_BEI_MIN <= prn && prn <= PRN_BEI_MAX) return Constellation::Beidou;
	if(PRN_QZS_MIN <= prn && prn <= PRN_QZS_MAX) return Constellation::Glonass;
	if(PRN_GAL_MIN <= prn && prn <= PRN_GAL_MAX) return Constellation::Galileo;

	switch (prn)
	{
		case 120: 	//EGNOS   Inmarsat 3f2 (AOR-E) 	 15.5° W
		case 124: 	//EGNOS   Artemis                21.5° E
		case 126: 	//EGNOS   Inmarsat 4f2 (IOR-W)   25.1° E
		case 127: 	//GAGAN   Inmarsat 4f1           64.0° E
		case 129: 	//MSAS    MTSAT 1R               140.0° E
		case 135: 	//WAAS 	  Galaxy 15 (Intelsat)   133.0° W
		case 137: 	//MSAS    MTSAT 2                145.0° E
		case 138: 	//WAAS    Anik F1R (Telesat)     107.3° W
			return Constellation::Sbas;
	}

	ALOGW("prn2constellation: invalid prn: %d", prn);
	return Constellation::Unknown;
}

static int16_t prn2svid(Constellation constellation, int16_t prn)
{
	switch(constellation)
	{
		case Constellation::Gps:
			return prn;

		case Constellation::Glonass:
			return prn - PRN_GLO_MIN + 1;

		case Constellation::Beidou:
			return prn - PRN_BEI_MIN + 1;

		case Constellation::Qzss:
			return prn;

		case Constellation::Galileo:
			return prn - PRN_GAL_MIN + 1;

		case Constellation::Sbas:
			return prn;

		case Constellation::Unknown:
		default:
			ALOGW("prn2svid: unknown constellation, returning prn as svid");
			return prn;
	}
}

static int16_t svid2prn(Constellation constellation, int16_t svid)
{
	switch(constellation)
	{
		case Constellation::Gps:
			return svid;

		case Constellation::Glonass:
			return svid + PRN_GLO_MIN - 1;

		case Constellation::Beidou:
			return svid + PRN_BEI_MIN - 1;

		case Constellation::Qzss:
			return svid;

		case Constellation::Galileo:
			return svid + PRN_GAL_MIN - 1;

		case Constellation::Sbas:
			return svid;

		case Constellation::Unknown:
		default:
			ALOGW("svid2prn: unknown constellation, returning svid as prn");
			return svid;
	}
}

SatIdentifier::SatIdentifier(int16_t prn) :
	prn(prn),
	constellation(prn2constellation(prn))
{
	this->svid = prn2svid(this->constellation, this->prn);
}

SatIdentifier::SatIdentifier(Constellation constellation, int16_t svid) :
	prn(svid2prn(constellation, svid)),
	svid(svid),
	constellation(constellation)
{ }

SatIdentifier::SatIdentifier(const SatIdentifier & other) :
	prn(other.prn),
	svid(other.svid),
	constellation(other.constellation)
{ }

SatIdentifier & SatIdentifier::operator = (const SatIdentifier & other)
{
	if(this != &other)
	{
		this->prn = other.prn;
		this->svid = other.svid;
		this->constellation = other.constellation;
	}

	return *this;
}

bool SatIdentifier::operator == (const SatIdentifier & other) const
{
	return &other == this || this->prn == other.prn;
}

bool SatIdentifier::operator != (const SatIdentifier & other) const
{
	return !(this->operator==(other));
}

bool SatIdentifier::operator < (const SatIdentifier & other) const
{
	return &other != this && this->prn < other.prn;
}

bool SatIdentifier::operator > (const SatIdentifier & other) const
{
	return &other != this && this->prn > other.prn;
}

bool SatIdentifier::operator <= (const SatIdentifier & other) const
{
	return !(this->operator>(other));
}

bool SatIdentifier::operator >= (const SatIdentifier & other) const
{
	return !(this->operator<(other));
}


} // namespace stm