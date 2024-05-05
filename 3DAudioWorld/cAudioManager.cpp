#include "pch.h"
#include "cAudioManager.h"
#include "cAudioUtil.h"

void cAudioManager::GLMToFMOD(const glm::vec3& in, FMOD_VECTOR& out)
{
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
}
void cAudioManager::FMODToGLM(const FMOD_VECTOR& in, glm::vec3& out)
{
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
}

//Constructor
cAudioManager::cAudioManager()
{
}

//Destructor
cAudioManager::~cAudioManager()
{
}

// Function to initialize the FMOD system
void cAudioManager::Initialize()
{
	if (mInitialized)
		return;

	FMOD_RESULT result;

	result = FMOD::System_Create(&mSystem);

	if (result != FMOD_OK)
	{
		printf("Failed to create FMOD System !\n");
		return;
	}

	result = mSystem->init(MAX_CHANNELS, FMOD_INIT_NORMAL | FMOD_INIT_PROFILE_ENABLE, nullptr);
	
	if (result != FMOD_OK)
	{
		printf("Failed to initialize system !");

		result = mSystem->close();

		if (result != FMOD_OK)
		{
			printf("Failed to close FMOD System !\n");
		}

		return;
	}

	printf("Audio Manager Initialized Successful !\n");

	for (int i = 0; i < MAX_CHANNELS; i++)
	{
		mChannelList.push_back(new Channel);
	}

	//------------------Initialize 3D settings--------------------//

	result = mSystem->set3DSettings(.5f, .5f, 0.1f);

	FMODCheckError(result);

	result = mSystem->setGeometrySettings(1000.0f);

	FMODCheckError(result);

	result = mSystem->createGeometry(2000, 8000, &mGeometry);

	FMODCheckError(result);

	mInitialized = true;
}

// Function to cleanup the system(release FMOD::SOUND instance/close and release FMOD::System instance)
void cAudioManager::Destroy()
{
	if (!mInitialized)
		return;

	FMOD_RESULT result;

	for (std::pair<const char*, Audio*>pair : mAudioMap)
	{
		result = pair.second->sound->release();
		FMODCheckError(result);
	}

	mAudioMap.clear();

	result = mSystem->close();
	FMODCheckError(result);

	result = mSystem->release();
	FMODCheckError(result);

	mInitialized = false;
}

// Function to load sound stream (loads audio resource data when needed)
void cAudioManager::LoadSoundStream(const char* file)
{
	if (!mInitialized)
		return;

	if (mAudioMap.find(file) != mAudioMap.end())
	{
		printf("AudioManager::LoadAudio() Audio already loaded!\n");
		return;
	}

	mAudioMap.insert(std::pair<const char*, Audio*>(file, new Audio()));

	FMOD_RESULT result;
	
	result = mSystem->createStream(file, FMOD_DEFAULT, 0, &mAudioMap[file]->sound);
	
	if (result != FMOD_OK)
	{
		printf("Failed to load the Audio file: %s\n", file);
	}

	printf("Audio : [%s] : Loaded successfully as stream !\n", file);
}

// Function to load sound stream (loads audio directly into memory)
void cAudioManager::LoadSound(const char* file)
{
	if (!mInitialized)
		return;

	if (mAudioMap.find(file) != mAudioMap.end())
	{
		printf("AudioManager::LoadAudio() Audio already loaded!\n");
		return;
	}

	mAudioMap.insert(std::pair<const char*, Audio*>(file, new Audio()));

	FMOD_RESULT result;
	
	result = mSystem->createSound(file, FMOD_DEFAULT, 0, &mAudioMap[file]->sound);
	
	if (result != FMOD_OK)
	{
		printf("Failed to load the Audio file: %s\n", file);
	}

	printf("Audio : [%s] : Loaded successfully into memory !\n", file);
}

// Function to load a media file directly into memory
void cAudioManager::Load3DAudio(const char* file)
{
	if (!mInitialized)
		return;

	if (mAudioMap.find(file) != mAudioMap.end())
	{
		printf("AudioManager::LoadAudio() Audio already loaded!\n");
		return;
	}

	mAudioMap.insert(std::pair<const char*, Audio*>(file, new Audio()));

	FMOD_RESULT result;

	result = mSystem->createSound(file, FMOD_3D | FMOD_LOOP_NORMAL, nullptr, &mAudioMap[file]->sound);
	
	if (result != FMOD_OK)
	{
		printf("Failed to load the Audio file: %s\n", file);
		return;
	}

	mAudioMap[file]->sound->set3DMinMaxDistance(0.5f, 1000.f);

	printf("3D Audio : [%s] : Loaded successfully into memory !\n", file);
}

// Function to initialize channel attributes
// [param_1]: Integer value is passed as parameter(channel id)
void cAudioManager::InitializeChannelAttributes(int id)
{
	mChannelList[id]->looping = false;
	mChannelList[id]->paused = false;
	mChannelList[id]->pitch = 1.f;
	mChannelList[id]->volume = 1.f;
	mChannelList[id]->pan = 0;
}

// Function to play the sound from path
// [param_1]: String value is passed as parameter(name/path of the audio to be played)
// [return_value]: The function returns an integer value(creates the channel id and returns it) 
int cAudioManager::Play3DSound(int id, const char* SoundName, FMOD_VECTOR soundPos, FMOD_VECTOR soundVel)
{
	if (!mInitialized)
	{
		printf("Not Initialized !\n");
		return m_NextChannelId;
	}

	std::map<const char*, Audio*>::iterator it = mAudioMap.find(SoundName);
	
	if (it == mAudioMap.end())
	{
		printf("Audio not found !\n");
		return m_NextChannelId;
	}

	FMOD_RESULT result;
	int channelId;

	if (id == 0 && m_NextChannelId == 0)
	{
		channelId = 0;
		m_NextChannelId = (m_NextChannelId + 1) % MAX_CHANNELS;
	}
	else if (id == 0)
	{
		channelId = m_NextChannelId;
		m_NextChannelId = (m_NextChannelId + 1) % MAX_CHANNELS;
	}
	else
		channelId = id;

	Channel* channel = mChannelList[channelId];

	result = mSystem->playSound(it->second->sound, nullptr, true, &channel->fmodChannel);
	FMODCheckError(result);

	mCurrentAudio = it->second;

	result = channel->fmodChannel->set3DAttributes(&soundPos, &soundVel);
	FMODCheckError(result);

	result = channel->fmodChannel->setPaused(false);
	FMODCheckError(result);

	return channelId;
}

// Function to check if the channel is still playing
// [param_1]: Integer value is passed as parameter(channel id)
// [return_value]: The function returns a bool value(true/false) 
bool cAudioManager::IsChannelPlaying(int id)
{
	bool isPLaying;

	mChannelList[id]->fmodChannel->isPlaying(&isPLaying);

	return isPLaying;
}

// Function to update the system. If any error indicated then the FMOD::System instance is released
void cAudioManager::Update()
{
	if (!mInitialized)
		return;

	FMOD_RESULT result;

	result = mSystem->update();

	if (result != FMOD_OK)
	{
		FMODCheckError(result);
		Destroy();
	}
}

// Function to pause and resume the channel audio
// [param_1]: Integer value is passed as parameter(channel id)
void cAudioManager::PauseSound(int id)
{
	FMOD_RESULT result;

	if (!mChannelList[id]->paused)
		mChannelList[id]->paused = true;
	else
		mChannelList[id]->paused = false;

	result = mChannelList[id]->fmodChannel->setPaused(mChannelList[id]->paused);
	FMODCheckError(result);
}

// Function to stop the channel audio
// [param_1]: Integer value is passed as parameter(channel id)
void cAudioManager::StopAudio(int id)
{
	FMOD_RESULT result;

	result = mChannelList[id]->fmodChannel->stop();
	FMODCheckError(result);
}

// Function to loop and unloop the channel audio
// [param_1]: Integer value is passed as parameter(channel id)
void cAudioManager::LoopAudio(int id)
{
	FMOD_RESULT result;

	if (mChannelList[id]->looping == false)
	{
		result = mChannelList[id]->fmodChannel->setMode(FMOD_LOOP_NORMAL);
		result = mChannelList[id]->fmodChannel->setLoopCount(-1);
		mChannelList[id]->looping = true;
	}
	else
	{
		result = mChannelList[id]->fmodChannel->setMode(FMOD_DEFAULT);
		result = mChannelList[id]->fmodChannel->setLoopCount(0);
		mChannelList[id]->looping = false;
	}
	FMODCheckError(result);
}

// Function to set the channel volume
// [param_1]: Integer value is passed as parameter(channel id)
// [param_2]: Float value is passed as parameter(value of volume to be changed in float)
void cAudioManager::SetChannelVolume(int id, float value)
{
	FMOD_RESULT result;
	mChannelList[id]->volume = value;

	result = mChannelList[id]->fmodChannel->setVolume(mChannelList[id]->volume);
	FMODCheckError(result);
}

// Function to set the channel pitch
// [param_1]: Integer value is passed as parameter(channel id)
// [param_2]: Float value is passed as parameter(value of pitch to be changed in float)
void cAudioManager::SetChannelPitch(int id, float value)
{
	FMOD_RESULT result;
	mChannelList[id]->pitch += value;

	if (mChannelList[id]->pitch < 0.2f)
	{
		mChannelList[id]->pitch = 0.2f;
	}

	result = mChannelList[id]->fmodChannel->setPitch(mChannelList[id]->pitch);
	FMODCheckError(result);
}

// Function to set the channel pan
// [param_1]: Integer value is passed as parameter(channel id)
// [param_2]: Float value is passed as parameter(value of pan to be changed in float)
void cAudioManager::SetChannelPan(int id, float value)
{
	FMOD_RESULT result;
	mChannelList[id]->pan += value;
	result = mChannelList[id]->fmodChannel->setPan(mChannelList[id]->pan);
	FMODCheckError(result);
}

// Function to get the channel volume
// [param_1]: Integer value is passed as parameter(channel id)
// [return_value]: The function returns a float value(channel volume is returned in float)
float cAudioManager::GetChannelVolume(int id)
{
	return mChannelList[id]->volume;
}

// Function to get the channel pitch
// [param_1]: Integer value is passed as parameter(channel id)
// [return_value]: The function returns a float value(channel pitch is returned in float)
float cAudioManager::GetChannelPitch(int id)
{
	return mChannelList[id]->pitch;
}

// Function to get the channel pan
// [param_1]: Integer value is passed as parameter(channel id)
// [return_value]: The function returns a float value(channel pan is returned in float)
float cAudioManager::GetChannelPan(int id)
{
	return mChannelList[id]->pan;
}

// Function to get the channel pause status
// [param_1]: Integer value is passed as parameter(channel id)
// [return_value]: The function returns a bool value(if channel is paused or not)
bool cAudioManager::GetPauseStatus(int id)
{
	return mChannelList[id]->paused;
}

// Function to get the channel loop status
// [param_1]: Integer value is passed as parameter(channel id)
// [return_value]: The function returns a bool value(if channel is looped or not)
bool  cAudioManager::GetLoopStatus(int id)
{
	return mChannelList[id]->looping;
}

// Function to get the channel audio's playback position
// [param_1]: Integer value is passed as parameter(channel id)
// [param_2]: Pointer of unsigned integer value is passed 
void cAudioManager::GetPlaybackPosition(int id, unsigned int& value)
{
	FMOD_RESULT result = mChannelList[id]->fmodChannel->getPosition(&value, FMOD_TIMEUNIT_MS);
	FMODCheckError(result);
}

// Function to get the audio length
// [param_1]: String value is passed as parameter(audio path)
// [param_2]: Pointer of unsigned integer value is passed 
void cAudioManager::GetAudioLength(const char* file, unsigned int& value)
{
	FMOD_RESULT result = mAudioMap[file]->sound->getLength(&value, FMOD_TIMEUNIT_MS);
	FMODCheckError(result);
}

void cAudioManager::UpdateSound3DAttributes(int channelId, FMOD_VECTOR soundPos, FMOD_VECTOR soundVel)
{
	FMOD_RESULT result;

	result = mChannelList[channelId]->fmodChannel->set3DAttributes(&soundPos, &soundVel);
	FMODCheckError(result);

}

//----------------------DSP Functions------------------------------------------

// Function to add the DSP Reverb Effect
// [param_1]: Integer value is passed as parameter(channel id)
void cAudioManager::AddReverbFilterOnChannel(int channelId, sAudioModels* audioModel)
{
	FMOD_RESULT result = mSystem->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &audioModel->reverbDSP);
	FMODCheckError(result);

	mChannelList[channelId]->fmodChannel->addDSP(0, audioModel->reverbDSP);
}

// Function to add the DSP Low Pass Filter Effect
// [param_1]: Integer value is passed as parameter(channel id)
void cAudioManager::AddLowPassFilterOnChannel(int channelId, sAudioModels* audioModel)
{
	FMOD_RESULT result = mSystem->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &audioModel->lowPassDSP);
	FMODCheckError(result);

	result = audioModel->lowPassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, 5000);
	FMODCheckError(result);

	mChannelList[channelId]->fmodChannel->addDSP(1, audioModel->lowPassDSP);
}

// Function to add the DSP High Pass Filter Effect
// [param_1]: Integer value is passed as parameter(channel id)
void cAudioManager::AddHighPassFilterOnChannel(int channelId, sAudioModels* audioModel)
{
	FMOD_RESULT result = mSystem->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &audioModel->highPassDSP);
	FMODCheckError(result);

	audioModel->highPassDSP->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, 500);
	FMODCheckError(result);

	mChannelList[channelId]->fmodChannel->addDSP(2, audioModel->highPassDSP);
}

// Function to add the DSP Distortion Effect
// [param_1]: Integer value is passed as parameter(channel id)
void cAudioManager::AddDistortionFilterOnChannel(int channelId, sAudioModels* audioModel)
{
	FMOD_RESULT result = mSystem->createDSPByType(FMOD_DSP_TYPE_DISTORTION, &audioModel->distortionDSP);
	FMODCheckError(result);

	audioModel->distortionDSP->setParameterFloat(FMOD_DSP_DISTORTION_LEVEL, 0.7f);
	FMODCheckError(result);

	mChannelList[channelId]->fmodChannel->addDSP(3, audioModel->distortionDSP);
}

// Function to add the DSP Chorus Effect
// [param_1]: Integer value is passed as parameter(channel id)
void cAudioManager::AddChorusPassOnChannel(int channelId, sAudioModels* audioModel)
{
	FMOD_RESULT result = mSystem->createDSPByType(FMOD_DSP_TYPE_CHORUS, &audioModel->chorusPassDSP);
	FMODCheckError(result);

	audioModel->chorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_MIX, 50.f);
	audioModel->chorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_RATE, 0.8f);
	audioModel->chorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_DEPTH, 3.f);
	FMODCheckError(result);

	mChannelList[channelId]->fmodChannel->addDSP(4, audioModel->chorusPassDSP);
}

// Function to set the DSP Reverb Effect
// [param_1]: Integer value is passed as parameter(channel id)
// [param_2]: Float value is passed as parameter(value of decay in float)
// [param_3]: Float value is passed as parameter(value of density in float)
// [param_4]: Float value is passed as parameter(value of diffusion in float)
void cAudioManager::SetReverbValuesOnChannel(int channelId, sAudioModels* audioModel, float decay, float density, float diffusion)
{
	FMOD_RESULT result;

	result = audioModel->reverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DECAYTIME, decay);
	result = audioModel->reverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DENSITY, density);
	result = audioModel->reverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DIFFUSION, diffusion);

	FMODCheckError(result);
}

// Function to set the DSP Low Pass Filter Effect
// [param_1]: Integer value is passed as parameter(channel id)
// [param_2]: Float value is passed as parameter(value of cutoff in float)
void cAudioManager::SetLowPassFilterValuesOnChannel(int channelId, sAudioModels* audioModel, float cutoff)
{
	FMOD_RESULT result = audioModel->lowPassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, cutoff);
	FMODCheckError(result);
}

// Function to set the DSP High Pass Filter Effect
// [param_1]: Integer value is passed as parameter(channel id)
// [param_2]: Float value is passed as parameter(value of cutoff in float)
void cAudioManager::SetHighPassFilterValuesOnChannel(int channelId, sAudioModels* audioModel, float cutoff)
{
	FMOD_RESULT result = audioModel->highPassDSP->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, cutoff);
	FMODCheckError(result);
}

// Function to set the DSP Distortion Effect
// [param_1]: Integer value is passed as parameter(channel id)
// [param_2]: Float value is passed as parameter(value of level in float)
void cAudioManager::SetDistortionLevelFilterValuesOnChannel(int channelId, sAudioModels* audioModel, float level)
{
	FMOD_RESULT result = audioModel->distortionDSP->setParameterFloat(FMOD_DSP_DISTORTION_LEVEL, level);
	FMODCheckError(result);
}

// Function to set the DSP Chorus Effect
// [param_1]: Integer value is passed as parameter(channel id)
// [param_2]: Float value is passed as parameter(value of mix in float)
// [param_3]: Float value is passed as parameter(value of rate in float)
// [param_4]: Float value is passed as parameter(value of depth in float)
void cAudioManager::SetChorusPassValuesOnChannel(int channelId, sAudioModels* audioModel, float mix, float rate, float depth)
{
	FMOD_RESULT result;

	result = audioModel->chorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_MIX, mix);
	result = audioModel->chorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_RATE, rate);
	result = audioModel->chorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_DEPTH, depth);
	FMODCheckError(result);
}

void cAudioManager::SetDopplerLevelEffect(int channelId, float level)
{
	FMOD_RESULT result;

	/*result = mChannelList[channelId]->fmodChannel->set3DLevel(level);
	FMODCheckError(result);*/

	result = mChannelList[channelId]->fmodChannel->set3DDopplerLevel(level);
	FMODCheckError(result);
}

//---------------------3D Functions-----------------------------------------

void cAudioManager::SetListenerAttributes(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up)
{
	float direct, reverb;

	FMOD_RESULT result;

	FMOD_VECTOR fmodPosition;
	FMOD_VECTOR fmodVelocity;
	FMOD_VECTOR fmodForward;
	FMOD_VECTOR fmodUp;
	FMOD_VECTOR origin;
	
	GLMToFMOD(position, fmodPosition);
	GLMToFMOD(velocity, fmodVelocity);
	GLMToFMOD(forward, fmodForward);
	GLMToFMOD(up, fmodUp);
	GLMToFMOD(glm::vec3(0.f), origin);

	result = mSystem->set3DListenerAttributes(0, &fmodPosition, &fmodVelocity, &fmodForward, &fmodUp);

/*	FMODCheckError(result); 

	if (result != FMOD_OK)
	{
		printf("Error!");
	}	*/

	result = mSystem->getGeometryOcclusion(&origin, &fmodPosition, &direct, &reverb);
	FMODCheckError(result);
	
	//printf("ListenerPosition: %.2f, %.2f, %.2f | direct: %.4f, reverb: %.4f\n", position.x, position.y, position.z, direct, reverb);
}

void cAudioManager::GetListenerAttributes(glm::vec3& position, glm::vec3& velocity, glm::vec3& forward, glm::vec3& up)
{
	FMOD_VECTOR fmodPosition;
	FMOD_VECTOR fmodVelocity;
	FMOD_VECTOR fmodForward;
	FMOD_VECTOR fmodUp;

	FMOD_RESULT result = mSystem->get3DListenerAttributes(0, &fmodPosition, &fmodVelocity, &fmodForward, &fmodUp);
	FMODCheckError(result);

	FMODToGLM(fmodPosition, position);
	FMODToGLM(fmodVelocity, velocity);
	FMODToGLM(fmodForward, forward);
	FMODToGLM(fmodUp, up);
}

bool CompareWithVertices(std::vector <FMOD_VECTOR> vertexSet, glm::vec3 approvedVertex)
{
	if (vertexSet.size() > 1)
	{
		for (int j = 0; j < vertexSet.size() - 1; j++)
		{
			if (approvedVertex.x != vertexSet[j].x || approvedVertex.y != vertexSet[j].y || approvedVertex.z != vertexSet[j].z)
			{
				if (j == vertexSet.size() - 2)
				{
					return true;
				}
			}
		}
	}
	else
		return true;

	return false;
}

int cAudioManager::AddPolygon(const std::vector<glm::vec3>& vertices, const glm::vec3& position)
{
	int index;
	int numVertices = vertices.size();

	glm::vec3 scale(1.f);
	glm::vec3 g;

	FMOD_RESULT result;

	FMOD_VECTOR fmodPosition;
	FMOD_VECTOR fmodScale;
	FMOD_VECTOR vertex;

	FMOD_VECTOR* fmodVertices = (FMOD_VECTOR*)malloc(sizeof(FMOD_VECTOR) * numVertices);
	
	for (int i = 0; i < numVertices; i++)
	{
		GLMToFMOD(vertices[i], fmodVertices[i]);
	}

	std::vector <FMOD_VECTOR> ordered_vertices;

	FMOD_VECTOR wall[4] = { {roundf(fmodVertices[0].x), roundf(fmodVertices[0].y), roundf(fmodVertices[0].z)},
							{roundf(fmodVertices[1].x), roundf(fmodVertices[1].y), roundf(fmodVertices[1].z)},
							{roundf(fmodVertices[2].x), roundf(fmodVertices[2].y), roundf(fmodVertices[2].z)},
							{roundf(fmodVertices[3].x), roundf(fmodVertices[3].y), roundf(fmodVertices[3].z)} };

	ordered_vertices.push_back(wall[0]);
	bool pushVertex = false;

	while (ordered_vertices.size() < 4)
	{
		for (int i = 1; i < 4; i++)
		{
			if (wall[i].x != ordered_vertices[ordered_vertices.size() - 1].x && wall[i].y == ordered_vertices[ordered_vertices.size() - 1].y && wall[i].z == ordered_vertices[ordered_vertices.size() - 1].z)
			{
				pushVertex = CompareWithVertices(ordered_vertices, glm::vec3(wall[i].x, wall[i].y, wall[i].z));

				if(pushVertex)
					ordered_vertices.push_back(wall[i]);
			}
			else if (wall[i].x == ordered_vertices[ordered_vertices.size()-1].x && wall[i].y != ordered_vertices[ordered_vertices.size()-1].y && wall[i].z == ordered_vertices[ordered_vertices.size()-1].z)
			{
				pushVertex = CompareWithVertices(ordered_vertices, glm::vec3(wall[i].x, wall[i].y, wall[i].z));

				if (pushVertex)
					ordered_vertices.push_back(wall[i]);
			}

			else if (wall[i].x == ordered_vertices[ordered_vertices.size()-1].x && wall[i].y == ordered_vertices[ordered_vertices.size()-1].y && wall[i].z != ordered_vertices[ordered_vertices.size()-1].z)
			{
				pushVertex = CompareWithVertices(ordered_vertices, glm::vec3(wall[i].x, wall[i].y, wall[i].z));

				if (pushVertex)
					ordered_vertices.push_back(wall[i]);
			}
		}
	}

	result = mGeometry->addPolygon(0.9f, 0.9f, true, ordered_vertices.size(), ordered_vertices.data(), &index);
	FMODCheckError(result);

	for (int i = 0; i < numVertices; i++)
	{
		mGeometry->getPolygonVertex(index, i, &vertex);

		FMODToGLM(vertex, g);
		
		printf("%.2f, %.2f, %.2f = %.2f, %.2f, %.2f\n", g.x, g.y, g.z, vertices[i].x, vertices[i].y, vertices[i].z);
	}

	mGeometry->setActive(true);
	return index;
}