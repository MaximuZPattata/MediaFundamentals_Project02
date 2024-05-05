#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class iPhysicsMeshTransformAccess
{
public:
	virtual ~iPhysicsMeshTransformAccess() {};

	virtual glm::vec3 getDrawPosition(void) = 0;
	virtual glm::vec3 getDrawOrientation(void) = 0;

	virtual void setDrawPosition(const glm::vec3& newPosition) = 0;
	virtual void setDrawOrientation(const glm::vec3& newOrientation) = 0;
	virtual void setDrawOrientation(const glm::quat& newOrientation) = 0;
};
