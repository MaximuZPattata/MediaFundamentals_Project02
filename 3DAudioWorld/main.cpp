#include "pch.h"

#include "cMesh.h"
#include "cJsonReader.h"
#include "cAudioManager.h"
#include "GLWF_CallBacks.h"

#include <cControlGameEngine.h>

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

//---------------------------Global Objects-----------------------------------------------
bool animationRunning = false;
double animationTime = glfwGetTime();
double timeLimit = 0.f;

GLFWwindow* window;
cControlGameEngine gameEngine;
cJsonReader jsonReader;
sCameraDetailsFromFile camDetails;
cAudioManager audioManager;
sAudioModels audioModels;

std::vector<sModelDetailsFromFile> modelDetailsList;
std::vector<sLightDetailsFromFile> lightDetailsList;
std::vector<sPhysicsDetailsFromFile> physicsDetailsList;
std::vector<sAudioModels> audioModelList;

// Function to end spaceship animation
void endAnimation(sAudioModels* spaceShipAudioModel);

// Function to move Spaceship
void moveSpaceship(sAudioModels* spaceShipAudioModel);

// Function to update Audio
void updateAudio()
{
    //-------------------Initialize---------------------------------------
    
    std::string modelName = "";

    glm::vec3 currentCamPosition = gameEngine.GetCurrentCameraPosition();
    glm::vec3 currentCamForward = gameEngine.GetCurrentCameraTarget();
    glm::vec3 currentCamUpVector = gameEngine.GetCurrentCameraUpVector();

    //-------------------Update Position----------------------------------

    audioManager.SetListenerAttributes(currentCamPosition, glm::vec3(0.f), currentCamForward, currentCamUpVector);

    //----------Compare distance and play Audio Model---------------------

    float offset_value = 90.0f;

    for (int audioIndex = 0; audioIndex < audioModelList.size(); audioIndex++)
    {
        sAudioModels* currentAudioModel = &audioModelList[audioIndex];

        modelName = currentAudioModel->modelName;

        if (modelName == "Spaceship")
        {
            if(currentAudioModel->isPlaying)
                moveSpaceship(currentAudioModel);
        }
        else
        {
            if (glm::distance(currentCamPosition, currentAudioModel->modelPosition) < offset_value)
            {
                float modelScale = gameEngine.GetModelScaleValue(modelName);

                if (modelScale <= currentAudioModel->modelScale)
                    modelScale = currentAudioModel->modelScale + 1.f;

                modelScale *= 0.99f;

                gameEngine.ScaleModel(modelName, modelScale);

                //----------------------------If Audio is not playing-------------------------------------

                if (!currentAudioModel->isPlaying)
                {
                    FMOD_VECTOR fmodPos;
                    FMOD_VECTOR fmodVel;

                    audioManager.GLMToFMOD(currentAudioModel->modelPosition, fmodPos);
                    audioManager.GLMToFMOD(currentAudioModel->modelVelocity, fmodVel);

                    int newChannelId = audioManager.Play3DSound(currentAudioModel->channelId, currentAudioModel->audioPath, fmodPos, fmodVel);

                    if (currentAudioModel->channelId == 0)
                        currentAudioModel->channelId = newChannelId;

                    currentAudioModel->isPlaying = true;

                    //------------------------Adding DSP Effects---------------------------------------------------------
                    
                    //------------------------If Channel = 1----------------------------------------------

                    if (currentAudioModel->channelId == 1)
                    {
                        audioManager.AddLowPassFilterOnChannel(currentAudioModel->channelId, currentAudioModel);
                        audioManager.AddHighPassFilterOnChannel(currentAudioModel->channelId, currentAudioModel);

                        audioManager.SetHighPassFilterValuesOnChannel(currentAudioModel->channelId, currentAudioModel, 200.f);
                        audioManager.SetLowPassFilterValuesOnChannel(currentAudioModel->channelId, currentAudioModel, 4500.f);
                    }

                    //------------------------If Channel = 2----------------------------------------------

                    else if (currentAudioModel->channelId == 2)
                    {
                        audioManager.AddLowPassFilterOnChannel(currentAudioModel->channelId, currentAudioModel);
                        audioManager.AddHighPassFilterOnChannel(currentAudioModel->channelId, currentAudioModel);

                        audioManager.SetHighPassFilterValuesOnChannel(currentAudioModel->channelId, currentAudioModel, 2000.f);
                        audioManager.SetLowPassFilterValuesOnChannel(currentAudioModel->channelId, currentAudioModel, 2000.f);
                    }

                    //------------------------If Channel = 3----------------------------------------------

                    else if (currentAudioModel->channelId == 3)
                    {
                        audioManager.AddReverbFilterOnChannel(currentAudioModel->channelId, currentAudioModel);
                        audioManager.SetReverbValuesOnChannel(currentAudioModel->channelId, currentAudioModel, 4000.f, 100.f, 100.f);
                    }

                    //------------------------If Channel = 4----------------------------------------------

                    else if (currentAudioModel->channelId == 4)
                    {
                        audioManager.AddDistortionFilterOnChannel(currentAudioModel->channelId, currentAudioModel);
                        audioManager.SetDistortionLevelFilterValuesOnChannel(currentAudioModel->channelId, currentAudioModel, 0.1f);
                    }

                    //------------------------If Channel = 5----------------------------------------------

                    else if (currentAudioModel->channelId == 5)
                    {
                        audioManager.AddChorusPassOnChannel(currentAudioModel->channelId, currentAudioModel);
                        audioManager.SetChorusPassValuesOnChannel(currentAudioModel->channelId, currentAudioModel, 90.f, 19.f, 90.f);
                    }
                }
            }
            else
            {
                if (currentAudioModel->isPlaying)
                {
                    audioManager.StopAudio(currentAudioModel->channelId);

                    gameEngine.ScaleModel(modelName, currentAudioModel->modelScale);

                    currentAudioModel->isPlaying = false;

                    printf("ModelName : %s | isPlaying : %d | ChannelId : %d\n", currentAudioModel->modelName, currentAudioModel->isPlaying, currentAudioModel->audioPath);
                }
            }
        }
    }

    audioManager.Update();
}

// Function to start spaceship animation
void beginAnimation();

int main()
{
    //-----------------------------------Initialize Window--------------------------------------

    int result = 0;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(800, 600, "Simple example", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //------------------------------Input key and Cursor initialize---------------------------------

    glfwSetKeyCallback(window, key_callback);

    glfwSetCursorPosCallback(window, mouse_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    //--------------------------------Initialize Game Engine----------------------------------------

    result = gameEngine.InitializeGameEngine();

    if (result != 0)
        return -1;

    //------------------------------Initialize and Load Audio---------------------------------------

    std::vector <std::string> audioPathList = { "Audio/Medieval_Music.wav", "Audio/Awesomeness.wav", "Audio/Spaceship.wav" };

    audioManager.Initialize();

    for(int index = 0; index < audioPathList.size(); index++)
        audioManager.Load3DAudio(audioPathList[index].c_str());

    //--------------------------------Loading Models, Lights and initial camera position from Json file---------------------------------------------

    bool jsonresult = jsonReader.ReadScene("SceneDescription.json", modelDetailsList, physicsDetailsList, lightDetailsList, camDetails);

    if (jsonresult)
    {
        std::cout << "File read successfully !" << std::endl;

        std::string modelName;
        int lightId;

        // Loading Models
        for (int index = 0; index < modelDetailsList.size(); index++)
        {
            modelName = modelDetailsList[index].modelName;

            gameEngine.LoadModelsInto3DSpace(modelDetailsList[index].modelFilePath, modelName,
                modelDetailsList[index].modelPosition.x, modelDetailsList[index].modelPosition.y, modelDetailsList[index].modelPosition.z);

            float angleRadians = glm::radians(modelDetailsList[index].modelOrientation.w);

            gameEngine.RotateMeshModel(modelName, angleRadians, modelDetailsList[index].modelOrientation.x,
                modelDetailsList[index].modelOrientation.y, modelDetailsList[index].modelOrientation.z);

            gameEngine.ScaleModel(modelName, modelDetailsList[index].modelScaleValue);

            if (modelDetailsList[index].wireframeModeOn)
                gameEngine.TurnWireframeModeOn(modelName);

            if (modelDetailsList[index].meshLightsOn)
                gameEngine.TurnMeshLightsOn(modelName);

            if (modelDetailsList[index].manualColors)
            {
                gameEngine.UseManualColors(modelName, true);
                gameEngine.ChangeColor(modelName, modelDetailsList[index].modelColorRGB.x, modelDetailsList[index].modelColorRGB.y, modelDetailsList[index].modelColorRGB.z);
            }
            
            //---------------------Adding occlusion to walls--------------------------------

            if (modelDetailsList[index].physicsMeshType == "Wall")
            {
                glm::vec3 modelPos = gameEngine.GetModelPosition(modelName);

                audioManager.AddPolygon(gameEngine.GetModelVertices(modelName), modelPos);
            }

            //----------------------------Loading Audio------------------------------------

            if (modelDetailsList[index].audioModel)
            {
                sAudioModels newAudioModel;

                newAudioModel.modelName = modelDetailsList[index].modelName.c_str();
                newAudioModel.isPlaying = false;
                newAudioModel.modelPosition = modelDetailsList[index].modelPosition;
                newAudioModel.modelScale = modelDetailsList[index].modelScaleValue;

                if (modelDetailsList[index].physicsMeshType == "Ship")
                {
                    newAudioModel.audioPath = audioPathList[2].c_str();
                    newAudioModel.modelVelocity = glm::vec3(0.f, 0.f, -5.0f);
                    newAudioModel.modelAcceleration = glm::vec3(0.f, 0.f, -9.8f);
                }
                else
                {
                    if (index % 2 == 0)
                        newAudioModel.audioPath = audioPathList[1].c_str();
                    else
                        newAudioModel.audioPath = audioPathList[0].c_str();
                }

                audioModelList.push_back(newAudioModel);
            }
        }

        // Loading Lights
        for (int index = 0; index < lightDetailsList.size(); index++)
        {
            lightId = lightDetailsList[index].lightId;

            gameEngine.CreateLight(lightId, lightDetailsList[index].lightPosition.x, lightDetailsList[index].lightPosition.y, lightDetailsList[index].lightPosition.z);
            gameEngine.ChangeLightType(lightId, lightDetailsList[index].lightType);
            gameEngine.ChangeLightIntensity(lightId, lightDetailsList[index].linearAttenuation, lightDetailsList[index].quadraticAttenuation);
            gameEngine.ChangeLightDirection(lightId, lightDetailsList[index].lightDirection.x, lightDetailsList[index].lightDirection.y, lightDetailsList[index].lightDirection.z);
            gameEngine.ChangeLightColour(lightId, lightDetailsList[index].lightColorRGB.r, lightDetailsList[index].lightColorRGB.g, lightDetailsList[index].lightColorRGB.b);
            gameEngine.ChangeLightAngle(lightId, lightDetailsList[index].innerAngle, lightDetailsList[index].outerAngle);

            if (lightDetailsList[index].lightOn)
                gameEngine.TurnOffLight(lightId, false);
            else
                gameEngine.TurnOffLight(lightId, true);
        }

        // Loading Initial Camera Position
        gameEngine.MoveCameraPosition(camDetails.initialCameraPosition.x, camDetails.initialCameraPosition.y, camDetails.initialCameraPosition.z);
    }

    else
        return -1;

    //-------------------------------Frame loop---------------------------------------------

    double lastTime = glfwGetTime();    

    while (!glfwWindowShouldClose(window))
    {
        //-------------------Calculate Delta time-------------------------------------------

        double currentTime = glfwGetTime();
        gameEngine.deltaTime = currentTime - lastTime;
        timeLimit = currentTime - animationTime;

        const double LARGEST_DELTA_TIME = 1.0f / 30.0f;

        if (gameEngine.deltaTime > LARGEST_DELTA_TIME)
            gameEngine.deltaTime = LARGEST_DELTA_TIME;

        lastTime = currentTime;

        if (timeLimit >= 7.5f && animationRunning == false)
        {
            animationTime = currentTime;
            animationRunning = true; 
            beginAnimation();
        }

        //--------------------Update Audio loop--------------------------------------------
        
        updateAudio();

        //--------------------Run Engine----------------------------------------------------

        gameEngine.RunGameEngine(window);
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    exit(EXIT_SUCCESS);

    std::cout << "Window closed !" << std::endl;

    return 0;
}