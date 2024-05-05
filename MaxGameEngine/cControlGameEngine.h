#pragma once
#include "cMesh.h"
#include "cPhysics.h"
#include "cLightManager.h"
#include "cLightHelper.h"
#include "cVAOManager.h"
#include "cShaderManager.h"

class cControlGameEngine
{
private:

    int gSelectedLight = 0;
    int meshListIndex = 0;
    int lightListIndex = 0;

    bool animationReversed = false;

    glm::vec3 cameraEye = glm::vec3(0.0, 0.0f, 0.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    GLuint shaderProgramID = 0;

    cShaderManager* mShaderManager = NULL;

    cVAOManager* mVAOManager = NULL;

    cPhysics* mPhysicsManager = NULL;

    cLightManager* mLightManager = NULL;

    std::vector < sPhysicsProperties* > PhysicsModelList;

    std::vector< cMesh* > TotalMeshList;

    std::vector < sModelDrawInfo* > MeshDrawInfoList;

    cShaderManager::cShader vertexShader;

    cShaderManager::cShader fragmentShader;

    void DrawObject(cMesh* pCurrentMesh, glm::mat4 matModel, GLuint shaderProgramID);

    cMesh* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

    sModelDrawInfo* g_pFindModelInfoByFriendlyName(std::string friendlyNameToFind);

    sPhysicsProperties* FindPhysicalModelByName(std::string modelName);

    int InitializeShader();

public:

    double deltaTime = 0.0f;
    float yaw = -90.0f;   // Vertical axis(Left and Right)
    float pitch = 0.0f;   // Horizontal axis(Up and Down)
    float previousX = 0.0f, previousY = 0.0f;
    bool mouseMoved = true;
    bool enableMouseMovement = false;

    //-------------------Common Functions--------------------------------------------------

    float getRandomFloat(float num1, float num2);

    //-------------------Camera Controls---------------------------------------------------

    void MoveCameraPosition(float translate_x, float translate_y, float translate_z);

    void MoveCameraTarget(float translate_x, float translate_y, float translate_z);

    glm::vec3 GetCurrentCameraPosition();

    glm::vec3 GetCurrentCameraTarget();

    glm::vec3 GetCurrentCameraUpVector();

    //-------------------Mesh Controls---------------------------------------------------

    void ChangeColor(std::string modelName, float r, float g, float b);

    void UseManualColors(std::string modelName, bool useColor);

    void ScaleModel(std::string modelName, float scale_value);

    void MoveModel(std::string modelName, float translate_x, float translate_y, float translate_z);

    glm::vec3 GetModelPosition(std::string modelName);

    float GetModelScaleValue(std::string modelName);

    void RotateMeshModel(std::string modelName, float angleRadians, float rotate_x, float rotate_y, float rotate_z);

    void TurnVisibilityOn(std::string modelName);

    void TurnWireframeModeOn(std::string modelName);

    void TurnMeshLightsOn(std::string modelName);

    std::vector<glm::vec3> GetModelVertices(std::string modelName);

    void DeleteMesh(std::string modelName);

    cMesh* ShiftToNextMeshInList();

    cMesh* ShiftToPreviousMeshInList();

    cMesh* GetCurrentModelSelected();

    //-------------------Light Controls---------------------------------------------------

    void CreateLight(int lightId, float initial_x, float initial_y, float initial_z);

    void TurnOffLight(int lightId, bool turnOff);

    void PositionLight(int lightId, float translate_x, float translate_y, float translate_z);

    void ChangeLightIntensity(int lightId, float linearAttentuation, float quadraticAttentuation);

    void ChangeLightType(int lightId, float lightType);

    void ChangeLightAngle(int lightId, float innerAngle, float outerAngle);

    void ChangeLightDirection(int lightId, float direction_x, float direction_y, float direction_z);

    void ChangeLightColour(int lightId, float color_r, float color_g, float color_b);

    float GetLightLinearAttenuation(int lightId);

    float GetLightQuadraticAttenuation(int lightId);

    float GetLightType(int lightId);

    float GetLightInnerAngle(int lightId);

    float GetLightOuterAngle(int lightId);

    float IsLightOn(int lightId);

    glm::vec3 GetLightPosition(int lightId);

    glm::vec3 GetLightDirection(int lightId);

    glm::vec3 GetLightColor(int lightId);

    void ShiftToNextLightInList();

    int GetCurrentLightSelected();

    //------------------Physics Controls---------------------------------------------------

    void ComparePhysicalAttributesWithOtherModels();

    void MakePhysicsHappen(sPhysicsProperties* physicsModel, std::string Model2, std::string collisionType);

    void ChangeModelPhysicsPosition(std::string modelName, float newPositionX, float newPositionY, float newPositionZ);

    void AddSpherePhysicsToMesh(std::string modelName, std::string physicsMeshType, float objectRadius);

    void AddPlanePhysicsToMesh(std::string modelName, std::string physicsMeshType);

    void ChangeModelPhysicsVelocity(std::string modelName, glm::vec3 velocityChange);

    void ChangeModelPhysicsAcceleration(std::string modelName, glm::vec3 accelerationChange);

    int ChangeModelPhysicalMass(std::string modelName, float mass);

    void ResetPosition(sPhysicsProperties* physicsModel);

    void AnimateTheCubes();

    //-------------------Engine Controls---------------------------------------------------

    void LoadModelsInto3DSpace(std::string filePath, std::string modelName, float initial_x, float initial_y, float initial_z);

    int InitializeGameEngine();

    void RunGameEngine(GLFWwindow* window);
};

