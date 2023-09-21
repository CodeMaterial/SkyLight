#pragma once

#include <string>

namespace skylight {

    class Sound {
    public:
        Sound(std::string card, std::string volumeControl, float volume);

        ~Sound();

        void PlayAudio(std::string filename);

        void Say(std::string message);

        void StopAll();

        void SetVolume(float volume);

        float GetVolume();

    private:

        std::string m_card = "";
        std::string m_volControl = "";

        float m_volume = 0.5;
    };
}

