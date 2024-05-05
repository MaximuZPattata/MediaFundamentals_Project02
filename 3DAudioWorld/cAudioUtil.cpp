#include "pch.h"
#include "cAudioUtil.h"

// Function to attend error calls especially with the FMOD integration
// [param_1]: The instance of enum FMOD_RESULT is passed
// [param_2]: A string value is passed consisting of the file path
// [param_3]: An integer is passed consisting of the line number
void CheckError(FMOD_RESULT result, const char* file, int line)
{
	if (result != FMOD_OK)
	{
		printf("FMOD Error [%d]: '%s' at %d\n", static_cast<int>(result), file, line);
	}
}