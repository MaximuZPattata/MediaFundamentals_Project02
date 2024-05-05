#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#include "iPhysicsMeshTransformAccess.h"

class cMesh : public iPhysicsMeshTransformAccess
{
private:

	glm::quat m_qOrientation;
	unsigned int m_UniqueID;
	static const unsigned int FIRST_UNIQUE_ID = 1000;
	static unsigned int m_nextUniqueID;

public:

	cMesh();
	~cMesh();

	bool bIsVisible;
	bool bIsWireframe;
	bool bDoNotLight;
	bool bUseManualColours;

	std::string meshName;

	std::string friendlyName;

	glm::vec3 drawPosition;

	glm::vec3 drawOrientation;

	glm::vec3 drawScale;

	glm::vec4 wholeObjectManualColourRGBA;

	std::vector<cMesh*> vec_pChildMeshes;

	void setRotationFromEuler(glm::vec3 newEulerAngleXYZ)
	{
		this->m_qOrientation = glm::quat(newEulerAngleXYZ);
	}

	void adjustRoationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust)
	{
		glm::quat qChange = glm::quat(EulerAngleXYZ_Adjust);
		this->m_qOrientation *= qChange;
	}

	void setUniformDrawScale(float scale);

	virtual void setDrawPosition(const glm::vec3& newPosition);
	virtual void setDrawOrientation(const glm::vec3& newOrientation);
	virtual void setDrawOrientation(const glm::quat& newOrientation);

	glm::quat get_qOrientation(void)
	{
		return this->m_qOrientation;
	}
	
	virtual glm::vec3 getDrawPosition(void);

	virtual glm::vec3 getDrawOrientation(void);

	unsigned int getUniqueID(void);
};

#endif

