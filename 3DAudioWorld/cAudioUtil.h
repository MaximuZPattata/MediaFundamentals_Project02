#pragma once
#include<fmod.hpp>

void CheckError(FMOD_RESULT result, const char* file, int line);

#define FMODCheckError(result) CheckError(result, __FILE__, __LINE__) //A substitute for CheckError() is created. 

