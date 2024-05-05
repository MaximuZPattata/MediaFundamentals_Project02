#include "cMesh.h"
#include <iostream>	

unsigned int cMesh::m_nextUniqueID = cMesh::FIRST_UNIQUE_ID;

cMesh::cMesh()
{
	this->drawPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	this->drawOrientation = glm::vec3(0.0f, 0.0f, 0.0f);
	this->setRotationFromEuler(glm::vec3(0.0f, 0.0f, 0.0f));
	this->wholeObjectManualColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->drawScale = glm::vec3(1.0f);

	this->bIsWireframe = false;
	this->bDoNotLight = false;
	this->bIsVisible = true;
	this->bUseManualColours = false;

	this->m_UniqueID = cMesh::m_nextUniqueID;

	cMesh::m_nextUniqueID++;
}

cMesh::~cMesh()
{

}

unsigned int cMesh::getUniqueID(void)
{
	return this->m_UniqueID;
}

void cMesh::setUniformDrawScale(float scale)
{
	this->drawScale.x = this->drawScale.y = this->drawScale.z = scale;
	return;
}

glm::vec3 cMesh::getDrawPosition(void)
{
	return this->drawPosition;
}

void cMesh::setDrawPosition(const glm::vec3& newPosition)
{
	this->drawPosition = newPosition;
	return;
}

glm::vec3 cMesh::getDrawOrientation(void)
{
	return glm::eulerAngles(this->get_qOrientation());
}

void cMesh::setDrawOrientation(const glm::vec3& newOrientation)
{
	this->setRotationFromEuler(newOrientation);
	return;
}

void cMesh::setDrawOrientation(const glm::quat& newOrientation)
{
	this->m_qOrientation = newOrientation;
	return;
}


