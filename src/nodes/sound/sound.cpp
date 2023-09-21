#include "sound.h"
#include <spdlog/spdlog.h>
#include <sstream>

skylight::Sound::Sound(std::string card, std::string volumeControl, float volume) {
    spdlog::info("skylight sound connecting to card: \"{}\" volume control:\"{}\" volume:\"{}\"", card, volumeControl,
                 volume);
    m_card = card;
    m_volControl = volumeControl;

    m_volume = volume;
    SetVolume(m_volume);
}

skylight::Sound::~Sound() {

}

void skylight::Sound::PlayAudio(std::string filename) {
    std::string command = "aplay -D default:CARD=" + m_card + " " + filename + " &";
    std::system(command.c_str());
}

void skylight::Sound::Say(std::string speech) {
    std::string command = "espeak '" + speech +
                          "' -z -s 190 --stdout | sox -t wav -r 22050 - -r 44100 -t wav - | aplay -D default:CARD=" +
                          m_card + " &";
    std::system(command.c_str());
}

void skylight::Sound::StopAll() {
    std::system("pkill aplay &");
}

void skylight::Sound::SetVolume(float volume) {
    if (volume > 1) volume = 1.0f;
    if (volume < 0) volume = 0.0f;

    std::stringstream ss;
    int volPercentage = static_cast<int>(volume * 100);
    ss << "amixer sset -c " << m_card << " " << m_volControl << " " << volPercentage << "% &";

    std::system(ss.str().c_str());

    m_volume = volume;
}