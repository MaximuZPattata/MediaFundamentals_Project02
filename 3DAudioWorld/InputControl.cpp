#include "GLWF_CallBacks.h"

#include <string>
#include <iostream>
#include <vector>

#include "cControlGameEngine.h"

extern cControlGameEngine gameEngine;
cMesh* controlMeshModel;

void resetCamAttributes()
{
    gameEngine.yaw = -90.0f;
    gameEngine.pitch = 0.0f;
    /*gameEngine.previousX = xPos;
    gameEngine.previousY = yPos;*/
    //gameEngine.mouseMoved = true;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    const float CAMERA_SPEED = 300.0f;

    const float MODEL_MOVE_SPEED = 1.0f;

    float CAMERA_MOVEMENT = CAMERA_SPEED * gameEngine.deltaTime;

    glm::vec3 CAMERA_TARGET = gameEngine.GetCurrentCameraTarget();
    glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 CAMERA_POSITION = gameEngine.GetCurrentCameraPosition();

    //-------------------Controls for Camera-----------------------------------------------------------

    if (mods == 0)
    {
        //------------------------------Camera Showcases------------------------------------------------

        if (key == GLFW_KEY_1 && action)
        {
            CAMERA_POSITION = glm::vec3(0.0f, 50.0f, 300.0f);
            gameEngine.MoveCameraTarget(0.0f, 0.0f, -1.0f);
            resetCamAttributes();
        }

        if (key == GLFW_KEY_2 && action)
        {
            CAMERA_POSITION = glm::vec3(125.0f, 200.0f, 305.0f);
            gameEngine.MoveCameraTarget(-0.30f, -0.35f, -0.90f);
            resetCamAttributes();
        }

        if (key == GLFW_KEY_3 && action)
        {
            CAMERA_POSITION = glm::vec3(-135.0f, 130.0f, 305.0f);
            gameEngine.MoveCameraTarget(0.50f, -0.20f, -0.85f);
            resetCamAttributes();
        }

        if (key == GLFW_KEY_4 && action)
        {
            CAMERA_POSITION = glm::vec3(-3.0f, 370.0f, -190.0f);
            gameEngine.MoveCameraTarget(0.005f, -0.70f, 0.70f);
            resetCamAttributes();
        }

        if (key == GLFW_KEY_SPACE && action)
        {
            if (gameEngine.enableMouseMovement)
                gameEngine.enableMouseMovement = false;
            else
                gameEngine.enableMouseMovement = true;

            resetCamAttributes();
        }

        //------------------------------Move Camera--------------------------------------------------------

        if (key == GLFW_KEY_W && action)
        {
            CAMERA_POSITION += CAMERA_MOVEMENT * CAMERA_TARGET;
        }
        if (key == GLFW_KEY_A && action)
        {
            CAMERA_POSITION -= glm::normalize(glm::cross(CAMERA_TARGET, CAMERA_UP)) * CAMERA_MOVEMENT;
        }
        if (key == GLFW_KEY_S && action)
        {
            CAMERA_POSITION -= CAMERA_MOVEMENT * CAMERA_TARGET;
        }
        if (key == GLFW_KEY_D && action)
        {
            CAMERA_POSITION += glm::normalize(glm::cross(CAMERA_TARGET, CAMERA_UP)) * CAMERA_MOVEMENT;
        }
        if (key == GLFW_KEY_Q && action)
        {
            CAMERA_POSITION += CAMERA_MOVEMENT * CAMERA_UP;
        }
        if (key == GLFW_KEY_E && action)
        {
            CAMERA_POSITION -= CAMERA_MOVEMENT * CAMERA_UP;
        }

        gameEngine.MoveCameraPosition(CAMERA_POSITION.x, CAMERA_POSITION.y, CAMERA_POSITION.z);
    }

    return;
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    if (gameEngine.enableMouseMovement)
    {
        glm::vec3 CAMERA_TARGET = gameEngine.GetCurrentCameraTarget();
        glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 CAMERA_POSITION = gameEngine.GetCurrentCameraPosition();

        int screenWidth, screenHeight;
        glfwGetWindowSize(window, &screenWidth, &screenHeight);

        float mouseSensitivity = 0.05f;

        float xOffset = 0.0f;
        float yOffset = 0.0f;

        if (xPos >= screenWidth - 1 || xPos <= 0) // Rotate Cam when mouse moves to the edge of the screen(left or right)
        {
            xOffset = 0.0f;

            if (xPos <= 0)
                xOffset = -3.0f;
            else if (xPos >= screenWidth - 1)
                xOffset = 3.0f;

            gameEngine.yaw += mouseSensitivity * xOffset;
        }

        if (yPos >= screenHeight - 1 || yPos <= 20) // Rotate Cam when mouse moves to the edge of the screen(up or down)
        {
            yOffset = 0.0f;

            if (yPos <= 20)
                yOffset = 3.0f;
            else if (yPos >= screenHeight - 1)
                yOffset = -3.0f;

            gameEngine.pitch += mouseSensitivity * yOffset;
        }

        if (gameEngine.mouseMoved)
        {
            gameEngine.previousX = xPos;
            gameEngine.previousY = yPos;
            gameEngine.mouseMoved = false;
        }

        xOffset = xPos - gameEngine.previousX;
        yOffset = gameEngine.previousY - yPos; //Reversed Y

        xOffset *= mouseSensitivity;
        yOffset *= mouseSensitivity;

        gameEngine.yaw += xOffset;
        gameEngine.pitch += yOffset;

        if (gameEngine.pitch > 90.0f)
            gameEngine.pitch = 90.0f;

        if (gameEngine.pitch < -90.0f)
            gameEngine.pitch = -90.0f;

        glm::vec3 front;

        front.x = cos(glm::radians(gameEngine.yaw)) * cos(glm::radians(gameEngine.pitch));
        front.y = sin(glm::radians(gameEngine.pitch));
        front.z = sin(glm::radians(gameEngine.yaw)) * cos(glm::radians(gameEngine.pitch));

        glm::vec3 cameraFront = glm::normalize(front);

        gameEngine.MoveCameraTarget(cameraFront.x, cameraFront.y, cameraFront.z);

        gameEngine.previousX = xPos;
        gameEngine.previousY = yPos;
    }
}