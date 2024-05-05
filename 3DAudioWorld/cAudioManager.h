#pragma once
#include "pch.h"
#include "cAudioUtil.h"

#include<map>
#include<vector>
#include <glm/glm.hpp>

struct sAudioModels
{
	bool isPlaying = false;

	int channelId = 0;
	float modelScale = 0;

	const char* modelName = "";
	const char* audioPath = "";

	FMOD::DSP* reverbDSP;
	FMOD::DSP* highPassDSP;
	FMOD::DSP* lowPassDSP;
	FMOD::DSP* distortionDSP;
	FMOD::DSP* chorusPassDSP;

	glm::vec3 modelPosition = glm::vec3(0.0f);
	glm::vec3 modelVelocity = glm::vec3(0.0f);
	glm::vec3 modelAcceleration = glm::vec3(0.0f);
};

// This struct is created to integrate with FMOD::Sound and handle audio related actions
struct Audio
{
	FMOD::Sound* sound; // Instance of FMOD Sound class
};

// This struct is created to integrate with FMOD::Channel and handle channel related actions
struct Channel
{
	FMOD::Channel* fmodChannel; // Instance of FMOD Channel class

	float volume; // Float value to hold the volume of the channel
	float pitch; // Float value to hold the pitch of the channel
	float pan; // Float value to hold the pan of the channel

	bool playing; // Bool value to check if the channel is playing
	bool looping = false; // Bool value to check if the channel is looping
	bool paused = false; // Bool value to check if the channel is paused
};

// This class is created to integrate with the FMOD system and manage sounds along with it's functionalities
class cAudioManager
{
private:
	const int MAX_CHANNELS = 10; // Integer variable to hold max number of channels
	int m_NextChannelId = 0; // Integer variable to hold channel id
	bool mInitialized = false; //Bool value to check if the system is initialized

	std::map<const char*, Audio*> mAudioMap; // Map to hold audio(Sound system)
	std::vector<Channel*> mChannelList; // Vector to hold Channel list

	Audio* mCurrentAudio; // Instance of Audio struct
	
	FMOD::System* mSystem = nullptr; // Instance of FMOD System class

	FMOD::Geometry* mGeometry; // Instance of FMOD Geometry class

public:
	cAudioManager();

	~cAudioManager();

	void Initialize();
	void Update();
	void Destroy();

	void SetChannelVolume(int id, float value);
	void SetChannelPitch(int id, float value);
	void SetChannelPan(int id, float value);
	void PauseSound(int id);
	void StopAudio(int id);
	void LoopAudio(int id);

	bool GetPauseStatus(int id);
	bool GetLoopStatus(int id);
	float GetChannelVolume(int id);
	float GetChannelPitch(int id);
	float GetChannelPan(int id);

	void LoadSoundStream(const char* source);
	void LoadSound(const char* file);
	int Play3DSound(int id, const char* SoundName, FMOD_VECTOR soundPos, FMOD_VECTOR soundVel);

	void InitializeChannelAttributes(int id);
	bool IsChannelPlaying(int id);
	void GetPlaybackPosition(int id, unsigned int& value);
	void GetAudioLength(const char* file, unsigned int& value);

	void UpdateSound3DAttributes(int channelId, FMOD_VECTOR fmodPos, FMOD_VECTOR fmodVel);

	//----------------DSP Functions--------------------------------

	void AddReverbFilterOnChannel(int channelId, sAudioModels* audioModel);
	void AddLowPassFilterOnChannel(int channelId, sAudioModels* audioModel);
	void AddHighPassFilterOnChannel(int channelId, sAudioModels* audioModel);
	void AddDistortionFilterOnChannel(int channelId, sAudioModels* audioModel);
	void AddChorusPassOnChannel(int channelId, sAudioModels* audioModel);

	void SetReverbValuesOnChannel(int channelId, sAudioModels* audioModel, float decay, float density, float diffusion);
	void SetLowPassFilterValuesOnChannel(int channelId, sAudioModels* audioModel, float limit);
	void SetHighPassFilterValuesOnChannel(int channelId, sAudioModels* audioModel, float limit);
	void SetDistortionLevelFilterValuesOnChannel(int channelId, sAudioModels* audioModel, float level);
	void SetChorusPassValuesOnChannel(int channelId, sAudioModels* audioModel, float mix, float rate, float depth);
	void SetDopplerLevelEffect(int channelId, float level); // 0.0f to 5.0f

	//----------------3D Functions---------------------------------

	void Load3DAudio(const char* file);
	 
	void SetListenerAttributes(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up);
	int AddPolygon(const std::vector<glm::vec3>& vertices, const glm::vec3& position);

	void GetListenerAttributes(glm::vec3& position, glm::vec3& velocity, glm::vec3& forward, glm::vec3& up);

	void GLMToFMOD(const glm::vec3& in, FMOD_VECTOR& out);
	void FMODToGLM(const FMOD_VECTOR& in, glm::vec3& out);
};
