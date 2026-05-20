#pragma once

// STL headers
#include <array>
#include <cstddef>

struct SVec3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	constexpr SVec3() = default;
	constexpr SVec3(float xValue, float yValue, float zValue): x(xValue), y(yValue), z(zValue) {}

	SVec3& operator+=(const SVec3& other);
	SVec3& operator-=(const SVec3& other);
	SVec3& operator*=(float scalar);
	SVec3& operator/=(float scalar);
};

struct SMat4
{
	std::array<std::array<float, 4>, 4> values {};

	constexpr SMat4() = default;
	explicit constexpr SMat4(float diagonal)
	{
		values[0][0] = diagonal;
		values[1][1] = diagonal;
		values[2][2] = diagonal;
		values[3][3] = diagonal;
	}

	std::array<float, 4>& operator[](std::size_t column);
	const std::array<float, 4>& operator[](std::size_t column) const;
	float* data();
	const float* data() const;
};

// Vector operators
SVec3 operator+(SVec3 lhs, const SVec3& rhs);
SVec3 operator-(SVec3 lhs, const SVec3& rhs);
SVec3 operator-(SVec3 value);
SVec3 operator*(SVec3 value, float scalar);
SVec3 operator*(float scalar, SVec3 value);
SVec3 operator/(SVec3 value, float scalar);

// Vector operations
float dot(const SVec3& lhs, const SVec3& rhs);
SVec3 cross(const SVec3& lhs, const SVec3& rhs);
float length(const SVec3& value);
SVec3 normalize(const SVec3& value);

// Angle conversion
float radians(float degrees);

// Common transformation functions
SMat4 identityMat4();
SMat4 operator*(const SMat4& lhs, const SMat4& rhs);
SMat4 translate(const SMat4& matrix, const SVec3& offset);
SMat4 scale(const SMat4& matrix, const SVec3& factor);
SMat4 rotate(const SMat4& matrix, float angleRadians, const SVec3& axis);
SMat4 perspective(float fovRadians, float aspectRatio, float nearPlane, float farPlane);
SMat4 lookAt(const SVec3& eye, const SVec3& center, const SVec3& up);