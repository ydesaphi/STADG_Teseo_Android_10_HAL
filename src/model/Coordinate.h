#ifndef TESEO_HAL_MODEL_COORDINATE
#define TESEO_HAL_MODEL_COORDINATE

#include "../utils/ByteVector.h"

namespace stm {

class DegreeMinuteCoordinate;

class DecimalDegreeCoordinate;

enum class CoordinateDirection : uint
{
	UNKNOWN,
	NORTH,
	SOUTH,
	EAST,
	WEST,
};

CoordinateDirection CoordinateDirectionParse(uint8_t byte);

class ICoordinate
{
public:
	virtual DegreeMinuteCoordinate asDegreeMinute() const = 0;
	virtual DecimalDegreeCoordinate asDecimalDegree() const = 0;

	virtual ~ICoordinate() { }
};

class DegreeMinuteCoordinate:
	public ICoordinate
{
private:
	int degree;
	double minute;
	CoordinateDirection direction;

public:
	DegreeMinuteCoordinate();

	DegreeMinuteCoordinate(const DegreeMinuteCoordinate & other);

	DegreeMinuteCoordinate(const DecimalDegreeCoordinate & other, bool lat = true);

	DegreeMinuteCoordinate(int degree, double minute, CoordinateDirection direction);

	DegreeMinuteCoordinate(const ByteVector & coordinate, uint8_t direction);

	int getDegree() const { return degree; }

	double getMinute() const { return minute; }

	CoordinateDirection getDirection() const { return direction; }

	DegreeMinuteCoordinate asDegreeMinute() const;

	DecimalDegreeCoordinate asDecimalDegree() const;
};

class DecimalDegreeCoordinate:
	public ICoordinate
{
private:
	double coordinate;

public:
	DecimalDegreeCoordinate();

	DecimalDegreeCoordinate(const DecimalDegreeCoordinate & other);

	DecimalDegreeCoordinate(const DegreeMinuteCoordinate & other);

	DecimalDegreeCoordinate(double coordinate);

	DecimalDegreeCoordinate(const ByteVector & coordinate);

	double value() const { return coordinate; }

	DegreeMinuteCoordinate asDegreeMinute() const;

	DecimalDegreeCoordinate asDecimalDegree() const;
};

} // namespace stm

#endif // TESEO_HAL_MODEL_COORDINATE