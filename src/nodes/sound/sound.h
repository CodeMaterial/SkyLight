#pragma once

#include <string>

namespace skylight {

    class Sound {
    public:
        Sound();

        ~Sound();

        bool Connect(std::string card, std::string volumeControl, float volume);

        void PlayAudio(std::string filename);

        void Say(std::string message);

        bool StopAll();

        bool SetVolume(float volume);

        float GetVolume();

    private:

        std::string m_card = "";
        std::string m_volControl = "";

        float m_volume = 0.5;
    };
}

