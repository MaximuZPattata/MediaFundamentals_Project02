#pragma once
#include "OpenGLCommon.h"

#include<rapidjson/filereadstream.h>
#include<rapidjson/stringbuffer.h>
#include<rapidjson/document.h>

#include<string>
#include<vector>

// This struct is created to imitate the model variables in the json file
struct sModelDetailsFromFile
{
	float modelScaleValue;
	bool meshLightsOn;
	bool wireframeModeOn;
	bool manualColors;
	bool audioModel;

	std::string modelName;
	std::string modelFilePath;
	std::string physicsMeshType;

	glm::vec3 modelPosition;
	glm::quat modelOrientation;
	glm::vec3 modelColorRGB;
};

// This struct is created to imitate the physics variables in the json file
struct sPhysicsDetailsFromFile
{
	float modelRadius;
	float modelMass;
	bool randomVelocity;

	std::string modelName;

	glm::vec3 modelVelocity;
	glm::vec3 modelAcceleration;
};

// This struct is created to imitate the light variables in the json file
struct sLightDetailsFromFile
{
	int lightId;
	float linearAttenuation;
	float quadraticAttenuation;
	float lightType;
	float innerAngle;
	float outerAngle;
	bool lightOn;

	glm::vec3 lightPosition;
	glm::vec3 lightDirection;
	glm::vec3 lightColorRGB;
};

// This struct is created to imitate the camera variables in the json file
struct sCameraDetailsFromFile
{
	glm::vec3 initialCameraPosition;
};

// This class is created to read from a Json file
class cJsonReader
{
public:

	bool ReadScene(const std::string& filePath, std::vector <sModelDetailsFromFile>& differentModelDetails,
		std::vector<sPhysicsDetailsFromFile>& differentPhysicsDetails, std::vector<sLightDetailsFromFile>& differentLightDetails,
		sCameraDetailsFromFile& camDetails);

private:

	rapidjson::Document ReadFile(std::string fileName);
};

