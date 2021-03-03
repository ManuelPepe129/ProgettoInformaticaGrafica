#include "bounds.h"
#include <algorithm>

BoundingRegion::BoundingRegion(BoundTypes type)
	: type(type)
{ }

BoundingRegion::BoundingRegion(glm::vec3 center, float radius)
	: type(BoundTypes::SPHERE), center(center), radius(radius)
{ }

BoundingRegion::BoundingRegion(glm::vec3 min, glm::vec3 max)
	:type(BoundTypes::AABB), min(min), max(max)
{ }

glm::vec3 BoundingRegion::calculateCenter()
{
	return (type == BoundTypes::AABB) ? (min + max) / 2.0f : center;
}

glm::vec3 BoundingRegion::calculateDimensions()
{
	return (type == BoundTypes::AABB) ? (max - min) : glm::vec3(2.0f * radius);
}

bool BoundingRegion::containsPoint(glm::vec3 pt)
{
	switch (type)
	{
	case BoundTypes::AABB:
		return (pt.x >= min.x) && (pt.x <= max.x) &&
			(pt.y >= min.y) && (pt.y <= max.y) &&
			(pt.z >= min.z) && (pt.z <= max.z);
		break;
	case BoundTypes::SPHERE:
		// x^2 + y^2 + z^2 <= r^2
		float distSquared = 0.0f;
		for (int i = 0; i < 3; i++)
		{
			distSquared += (center[i] - pt[i]) * (center[i] - pt[i]);
		}
		return distSquared <= (radius * radius);
		break;
	}
	return false;
}

bool BoundingRegion::containsRegion(BoundingRegion br)
{
	if (br.type == BoundTypes::AABB) 
	{
		// if br is a box, jus has to contain min and max
		return containsPoint(br.min) && containsPoint(br.max);
	}
	else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE)
	{
		// if both spheres, combination of distance from centers and br.radius < radius
		return glm::length(center - br.center) + br.radius <= radius;
	}
	else
	{
		// if this is a box and br a sphere
		if (!containsPoint(br.center))
		{
			return false;
		}

		// center inside the box
		/*
		* for each axis (x, y, z)
		*	if distance to each side is smaller than the radius, return false
		*/
		for (int i = 0; i < 3; i++)
		{
			if (abs(max[i] - br.center[i]) < br.radius || abs(br.center[i] - min[i]) < br.radius)
			{
				return false;
			}
		}

		return true;
	}
}

bool BoundingRegion::intersects(BoundingRegion br)
{
	if (type == BoundTypes::AABB && br.type == BoundTypes::AABB)
	{
		// both boxes
		glm::vec3 rad = calculateDimensions() / 2.0f; // "radius" of this box
		glm::vec3 brRad = br.calculateDimensions() / 2.0f; // "radius" of br

		glm::vec3 center = calculateCenter();
		glm::vec3 brCenter = br.calculateCenter();

		glm::vec3 dist = abs(center - brCenter);

		for (int i = 0; i < 3; ++i) 
		{
			if (dist[i] > rad[i] + brRad[i])
			{
				// no overlap on this axis
				return false;
			}
		}

		return true;
	}
	else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE)
	{
		// both spheres - distance between centers must be less than combined radius
		return glm::length(center - br.center) < (radius + br.radius);
	}
	else if (type == BoundTypes::SPHERE)
	{
		// this is a sphere, br is a box
		float distSquared = 0.0f;
		for (int i = 0; i < 3; i++) 
		{
			// determine closest side
			float closestPt = std::max(br.min[i], std::min(center[i], br.max[i]));
			// add distance
			distSquared += (closestPt - center[i]) * (closestPt - center[i]);
		}

		return distSquared < (radius * radius);
	}
	else
	{
		// this is a box, br is a sphere
		return br.intersects(*this);
	}
}
