#ifndef SONIDO_H
#define SONIDO_H

#include "../Rosalila.h"

#include <string>
#include <vector>

#include <iostream>
using namespace std;

//SDL
#include "SDL2/SDL.h"

#ifdef LINUX
#include <SDL2/SDL_mixer.h>
#endif

#ifdef WINDOWS
#include <SDL2/SDL_mixer.h>
#endif

#ifdef OSX
#include <SDL2_mixer/SDL_mixer.h>
#endif

#include "../RosalilaUtility/RosalilaUtility.h"
#include <map>

class RosalilaSound
{
public:
    std::map<std::string,Mix_Chunk*> sounds;
    Mix_Music *music;
    string current_music;
    RosalilaSound();
    void drop();
    void addSound(std::string variable,std::string value);
    bool soundExists(std::string variable);
    int playSound(std::string variable, int channel,int loops);
    void playMusic(std::string variable,int loops);
    void stopMusic();
};

#endif
