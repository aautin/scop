#include "geometric.h"

float epsilon()
{
	return std::numeric_limits<float>::epsilon();
}

static SVertex crossProduct(const SVertex& a, const SVertex& b)
{
	float x = a.y * b.z - a.z * b.y;
	float y = a.z * b.x - a.x * b.z;
	float z = a.x * b.y - a.y * b.x;

	return { x, y, z };
}

static float dotProduct(const SVertex& a, const SVertex& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

bool areCoplanar(const SVertex& a, const SVertex& b, const SVertex& c, const SVertex& d)
{
	SVertex ab = { b.x - a.x, b.y - a.y, b.z - a.z };
	SVertex ac = { c.x - a.x, c.y - a.y, c.z - a.z };
	SVertex ad = { d.x - a.x, d.y - a.y, d.z - a.z };

	return std::abs(dotProduct(crossProduct(ab, ad), ac)) < epsilon();
}

bool areConvex(const SVertex& a, const SVertex& b, const SVertex& c, const SVertex& d)
{
	
}

STriangles getTriangles(const SVertex& a, const SVertex& b, const SVertex& c, const SVertex& d)
{
	if (!areCoplanar(a, b, c, d))
	{
		return STriangles({
			{ 0, 1, 2 },
			{ 0, 2, 3 },
			{ 0, 1, 3 },
			{ 1, 2, 3 },
		});
	}

	if (areConvex(a, b, c, d))
	{
		return STriangles({
			{ 0, 1, 2 },
			{ 0, 2, 3 },
		});
	}
	else
	{
		//
		// To be continued
		//
	}
}