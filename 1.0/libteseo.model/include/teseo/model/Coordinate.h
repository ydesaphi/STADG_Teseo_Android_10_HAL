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
/**
 * @file Coordinate.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#ifndef TESEO_HAL_MODEL_COORDINATE
#define TESEO_HAL_MODEL_COORDINATE

#include <teseo/utils/ByteVector.h>

namespace stm {

class DegreeMinuteCoordinate;

class DecimalDegreeCoordinate;

/**
 * @brief      Coordinate direction enumeration
 */
enum class CoordinateDirection : uint8_t
{
	UNKNOWN,
	NORTH,
	SOUTH,
	EAST,
	WEST,
};

/**
 * @brief      Parse ascii char to coordinate direction
 *
 * @param[in]  byte  The byte to parse
 *
 * @return     The coordinate direction, or UNKNOWN if character is not N, S, E nor W.
 */
CoordinateDirection CoordinateDirectionParse(uint8_t byte);

/**
 * @brief      Coordinate interface
 */
class ICoordinate
{
public:
	/**
	 * @brief      Convert coordinate to degree minute coordinate
	 *
	 * @return     New degree minute coordinate object
	 */
	virtual DegreeMinuteCoordinate asDegreeMinute() const = 0;

	/**
	 * @brief      Convert coordinate to decimal degree coordinate
	 *
	 * @return     New decimal degree coordinate
	 */
	virtual DecimalDegreeCoordinate asDecimalDegree() const = 0;

	virtual ~ICoordinate() { }
};

/**
 * @brief      Degree minute coordinate class
 * @details    A degre minute coordinate store the degree as an integer, minute as decimal minutes
 * (double) and the coordinate direction.
 */
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

	/**
	 * @brief      Create coordinate from ascii string
	 *
	 * @param[in]  coordinate  The coordinate string
	 * @param[in]  direction   The direction character
	 */
	DegreeMinuteCoordinate(const ByteVector & coordinate, uint8_t direction);

	int getDegree() const { return degree; }

	double getMinute() const { return minute; }

	CoordinateDirection getDirection() const { return direction; }

	DegreeMinuteCoordinate asDegreeMinute() const;

	DecimalDegreeCoordinate asDecimalDegree() const;
};

/**
 * @brief      Decimal degree coordinate class
 * @details    A decimal degree coordinate store the coordinate as a double. Direction is indicated
 * by the sign.
 */
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

	/**
	 * @brief      Create coordinate from ascii string
	 *
	 * @param[in]  coordinate  The coordinate string
	 */
	DecimalDegreeCoordinate(const ByteVector & coordinate);

	double value() const { return coordinate; }

	DegreeMinuteCoordinate asDegreeMinute() const;

	DecimalDegreeCoordinate asDecimalDegree() const;
};

} // namespace stm

#endif // TESEO_HAL_MODEL_COORDINATE