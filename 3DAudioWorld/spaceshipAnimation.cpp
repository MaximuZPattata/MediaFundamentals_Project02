#include "pch.h"

#include "cAudioManager.h"
#include <cControlGameEngine.h> 

extern bool animationRunning;
extern double animationTime;
extern double timeLimit;

extern cControlGameEngine gameEngine;

extern cAudioManager audioManager;

extern  std::vector<sAudioModels> audioModelList;

// Function to end animation
void endAnimation(sAudioModels* spaceShipAudioModel)
{
    if (spaceShipAudioModel->isPlaying)
    {
        audioManager.StopAudio(spaceShipAudioModel->channelId);

        spaceShipAudioModel->isPlaying = false;
    }

    spaceShipAudioModel->modelVelocity = glm::vec3(0.0f, 0.0f, -5.0f);
    spaceShipAudioModel->modelAcceleration = glm::vec3(0.0f, 0.0f, -9.8f);

    gameEngine.MoveModel(spaceShipAudioModel->modelName, spaceShipAudioModel->modelPosition.x, spaceShipAudioModel->modelPosition.y, spaceShipAudioModel->modelPosition.z);

    animationRunning = false;
    animationTime = glfwGetTime();
    timeLimit = 0.f;
}

// Function to move Spaceship
void moveSpaceship(sAudioModels* spaceShipAudioModel)
{
    glm::vec3 velocityChange = spaceShipAudioModel->modelAcceleration * (float)gameEngine.deltaTime;

    spaceShipAudioModel->modelVelocity += velocityChange;

    glm::vec3 positionChange = spaceShipAudioModel->modelVelocity * (float)gameEngine.deltaTime;
    glm::vec3 modelPos = gameEngine.GetModelPosition(spaceShipAudioModel->modelName);

    modelPos.x += positionChange.x;
    modelPos.y += positionChange.y;
    modelPos.z += positionChange.z;

    gameEngine.MoveModel(spaceShipAudioModel->modelName, modelPos.x, modelPos.y, modelPos.z);

    FMOD_VECTOR fmodPos;
    FMOD_VECTOR fmodVel;

    audioManager.GLMToFMOD(modelPos, fmodPos);
    audioManager.GLMToFMOD(spaceShipAudioModel->modelVelocity, fmodVel);

    audioManager.UpdateSound3DAttributes(spaceShipAudioModel->channelId, fmodPos, fmodVel);

    if (modelPos.z < -500.0f)
        endAnimation(spaceShipAudioModel);
}

// Function to start spaceship animation
void beginAnimation()
{
    for (int i = 0; i < audioModelList.size(); i++)
    {
        std::string modelName = audioModelList[i].modelName;
        std::string compareModelName = "Spaceship";

        if (modelName == compareModelName)
        {
            sAudioModels* spaceShipAudioModel = &audioModelList[i];

            if (!spaceShipAudioModel->isPlaying)
            {
                FMOD_VECTOR fmodPos;
                FMOD_VECTOR fmodVel;

                audioManager.GLMToFMOD(spaceShipAudioModel->modelPosition, fmodPos);
                audioManager.GLMToFMOD(spaceShipAudioModel->modelVelocity, fmodVel);

                int newChannelId = audioManager.Play3DSound(spaceShipAudioModel->channelId, spaceShipAudioModel->audioPath, fmodPos, fmodVel);

                if (spaceShipAudioModel->channelId == 0)
                    spaceShipAudioModel->channelId = newChannelId;

                audioManager.SetChannelVolume(spaceShipAudioModel->channelId, 1.0f);
                audioManager.SetDopplerLevelEffect(spaceShipAudioModel->channelId, 4.f);

                spaceShipAudioModel->isPlaying = true;
            }
        }
    }
}