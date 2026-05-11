#include "math3d.h"

// STL headers
#include <cmath>
#include <numbers>
#include <stdexcept>

SVec3& SVec3::operator+=(const SVec3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

SVec3& SVec3::operator-=(const SVec3& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

SVec3& SVec3::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

SVec3& SVec3::operator/=(float scalar)
{
	if (scalar == 0.0f)
	{
		throw std::runtime_error("Division by zero in SVec3");
	}
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}

std::array<float, 4>& SMat4::operator[](std::size_t column)
{
	return values[column];
}

const std::array<float, 4>& SMat4::operator[](std::size_t column) const
{
	return values[column];
}

float* SMat4::data()
{
	return &values[0][0];
}

const float* SMat4::data() const
{
	return &values[0][0];
}

SVec3 operator+(SVec3 lhs, const SVec3& rhs)
{
	lhs += rhs;
	return lhs;
}

SVec3 operator-(SVec3 lhs, const SVec3& rhs)
{
	lhs -= rhs;
	return lhs;
}

SVec3 operator-(SVec3 value)
{
	return {-value.x, -value.y, -value.z};
}

SVec3 operator*(SVec3 value, float scalar)
{
	value *= scalar;
	return value;
}

SVec3 operator*(float scalar, SVec3 value)
{
	value *= scalar;
	return value;
}

SVec3 operator/(SVec3 value, float scalar)
{
	value /= scalar;
	return value;
}

float dot(const SVec3& lhs, const SVec3& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

SVec3 cross(const SVec3& lhs, const SVec3& rhs)
{
	return {
		lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.z * rhs.x - lhs.x * rhs.z,
		lhs.x * rhs.y - lhs.y * rhs.x,
	};
}

float length(const SVec3& value)
{
	return std::sqrt(dot(value, value));
}

SVec3 normalize(const SVec3& value)
{
	float vecLength = length(value);
	if (vecLength == 0.0f)
	{
		return {};
	}
	return value / vecLength;
}

float radians(float degrees)
{
	return degrees * static_cast<float>(std::numbers::pi_v<double> / 180.0);
}

SMat4 identityMat4()
{
	return SMat4{1.0f};
}

SMat4 operator*(const SMat4& lhs, const SMat4& rhs)
{
	SMat4 result;
	for (std::size_t column = 0; column < 4; ++column)
	{
		for (std::size_t row = 0; row < 4; ++row)
		{
			for (std::size_t index = 0; index < 4; ++index)
			{
				result[column][row] += lhs[index][row] * rhs[column][index];
			}
		}
	}
	return result;
}

SMat4 translate(const SMat4& matrix, const SVec3& offset)
{
	SMat4 translation = identityMat4();
	translation[3][0] = offset.x;
	translation[3][1] = offset.y;
	translation[3][2] = offset.z;
	return matrix * translation;
}

SMat4 scale(const SMat4& matrix, const SVec3& factor)
{
	SMat4 scaling = identityMat4();
	scaling[0][0] = factor.x;
	scaling[1][1] = factor.y;
	scaling[2][2] = factor.z;
	return matrix * scaling;
}

SMat4 rotate(const SMat4& matrix, float angleRadians, const SVec3& axis)
{
	SVec3 unitAxis = normalize(axis);
	if (length(unitAxis) == 0.0f)
	{
		return matrix;
	}

	float cosine = std::cos(angleRadians);
	float sine = std::sin(angleRadians);
	float oneMinusCosine = 1.0f - cosine;

	SMat4 rotation = identityMat4();
	rotation[0][0] = cosine + unitAxis.x * unitAxis.x * oneMinusCosine;
	rotation[0][1] = unitAxis.x * unitAxis.y * oneMinusCosine + unitAxis.z * sine;
	rotation[0][2] = unitAxis.x * unitAxis.z * oneMinusCosine - unitAxis.y * sine;
	rotation[0][3] = 0.0f;

	rotation[1][0] = unitAxis.y * unitAxis.x * oneMinusCosine - unitAxis.z * sine;
	rotation[1][1] = cosine + unitAxis.y * unitAxis.y * oneMinusCosine;
	rotation[1][2] = unitAxis.y * unitAxis.z * oneMinusCosine + unitAxis.x * sine;
	rotation[1][3] = 0.0f;

	rotation[2][0] = unitAxis.z * unitAxis.x * oneMinusCosine + unitAxis.y * sine;
	rotation[2][1] = unitAxis.z * unitAxis.y * oneMinusCosine - unitAxis.x * sine;
	rotation[2][2] = cosine + unitAxis.z * unitAxis.z * oneMinusCosine;
	rotation[2][3] = 0.0f;

	rotation[3][0] = 0.0f;
	rotation[3][1] = 0.0f;
	rotation[3][2] = 0.0f;
	rotation[3][3] = 1.0f;

	return matrix * rotation;
}

SMat4 perspective(float fovRadians, float aspectRatio, float nearPlane, float farPlane)
{
	float tangent = std::tan(fovRadians / 2.0f);
	if (tangent == 0.0f || aspectRatio == 0.0f || nearPlane == farPlane)
	{
		throw std::runtime_error("Invalid perspective parameters");
	}

	SMat4 result;
	result[0][0] = 1.0f / (aspectRatio * tangent);
	result[1][1] = 1.0f / tangent;
	result[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
	result[2][3] = -1.0f;
	result[3][2] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
	return result;
}

SMat4 lookAt(const SVec3& eye, const SVec3& center, const SVec3& up)
{
	SVec3 forward = normalize(center - eye);
	SVec3 side = normalize(cross(forward, up));
	SVec3 cameraUp = cross(side, forward);

	SMat4 result = identityMat4();
	result[0][0] = side.x;
	result[0][1] = cameraUp.x;
	result[0][2] = -forward.x;
	result[0][3] = 0.0f;

	result[1][0] = side.y;
	result[1][1] = cameraUp.y;
	result[1][2] = -forward.y;
	result[1][3] = 0.0f;

	result[2][0] = side.z;
	result[2][1] = cameraUp.z;
	result[2][2] = -forward.z;
	result[2][3] = 0.0f;

	result[3][0] = -dot(side, eye);
	result[3][1] = -dot(cameraUp, eye);
	result[3][2] = dot(forward, eye);
	result[3][3] = 1.0f;

	return result;
}