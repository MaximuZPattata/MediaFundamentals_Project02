<<-------------------------------------------------------------------------------Media_Fundamentals_Project_02------------------------------------------------------------------------------------------------->>
# About Project :

	- The project reads the scene contents( i.e the model properties, position, light properties, position etc) from the json file located in "3D_Audio_World->3DAudioWorld->SceneDescription.json". The project loads the models and the light settings as mentioned in the human readable file and displays a sensible scene. 

	- I have created a not too fancy world, yet a world filled with audio elements. There are 6 spheres/orbs split equally(3 on the left and 3 on the right). Each orb emits a sound when the user gets close by. Some sounds maybe the same but they still have different DSP effects making them unique.

	- There is also a wall in front of each orb. When moving behind the wall while the orb emits sound will oppress the sound emitted by the orb due to the audio occlusion set on the wall. 

	- Also every few minutes a spaceship goes for a round above the created world. The ship emits a sound with added Doppler effect. 
 
	- The json file holds the scene description and changing the json file will change the scene. But the format of the json file should be the same to load the scene. 

## How to build and run : 

	- The solution "3D_Audio_World.sln" has to be built(either in debug or release platform mode) and then once succeeded(probably will!), make the "3DAudioWorld" project as the startup project and now it'll be able to run for some visual treat. 
	
### Controls :

	- W, A, S, D, Q, E = Move camera Front, Back, Left, Right, Up and Down respectively. 
	- Mouse movement = Camera can be rotated using the mouse.
	- Spacebar = If incase the user thinks the cam rotation is in the way of examining the scene, Spacebar pauses and resumes the mouse movement(By default the cam rotation is in a pause, so when the scene 			loads, press Spacebar to activate camera rotation).

#### Marks implemented : [All the marks are implemented through the main() and the cAudioManager class]
	- Scene created
	- 3D Listener Attributes : Updated Listener Attributes through the audio manager
	- User Interaction : The spheres start playing only after the user/camera is close enough to the object(Not sure if that comes under user interaction)
	- 3D Audio Setup with FMOD : FMOD used to manage and play various 3D sounds and effects
	- Object Based Occlusion : Placed 6 walls(single planes) in the scene to act as occlusion
	- Doppler Effect : Added a spaceship to run accross the scene with medium paced velocity to give out a sound with doppler effect
	- DSP Effects : Each sphere contributes to a different DSP effect like(Reverb, High and Low level filter, Distortion, Chorus). It is implemented in the updateAudio() function written in the main.cpp for each channel id. 
	- Bonus : Used distance formula to detect when user is closer to the audio source and play audio with also setting the 3D Listener Attributes.   
	
##### Limitations to the project : 

	- Although the mouse camera rotation is a bit effective than the keypad, it's still not perfected. There are some flaws like, when the mouse movement is paused and resuming the mouse movement after moving the cam position, the rotation seems to be irregular at first and then gets used to the new perspective. 
	- The project is not too fancy and my scene may not contribute to an actual artistic scene. Will make it better and ready for my portfolio soon ! :) 
