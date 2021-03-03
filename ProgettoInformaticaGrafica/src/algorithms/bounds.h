#pragma once

#include <glm/glm.hpp>

enum class BoundTypes {
	AABB, // axis aligned bounding box
	SPHERE
};

class BoundingRegion
{
public:

	// initialize with type
	BoundingRegion(BoundTypes type);
	BoundingRegion(glm::vec3 center, float radius);
	BoundingRegion(glm::vec3 min, glm::vec3 max);

	/*
	*/

	// center
	glm::vec3 calculateCenter();

	// dimensions
	glm::vec3 calculateDimensions();

	/*
		testing methods
	*/

	bool containsPoint(glm::vec3 pt);
	bool containsRegion(BoundingRegion br);

	bool intersects(BoundingRegion br);

	BoundTypes type;

	// sphere values
	glm::vec3 center;
	float radius;

	// box values
	glm::vec3 min;
	glm::vec3 max;
};

