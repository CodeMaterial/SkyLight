#include "speech.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <iostream>


skylight::Speech::Speech(std::string audioDevice) {

    spdlog::info("skylight speech connecting to {}", audioDevice);
    static const arg_t cont_args_def[] = {POCKETSPHINX_OPTIONS,
                                          {"-adcdev", ARG_STRING, NULL, "Name of audio device to use for input."},
                                          CMDLN_EMPTY_OPTION};
    m_pConfig = cmd_ln_init(NULL, cont_args_def, FALSE, "-inmic", "yes", "-adcdev", audioDevice.c_str(), NULL);
    ps_default_search_args(m_pConfig);

    if (m_pConfig == NULL) {
        throw std::runtime_error("skylight speech failed to setup config ");
    }

    m_pSpeechDecoder = ps_init(m_pConfig);

    if (m_pSpeechDecoder == NULL) {
        throw std::runtime_error("skylight speech failed to initialise the speech decoder");
    }

    m_pDevice = ad_open_dev(cmd_ln_str_r(m_pConfig, "-adcdev"), (int) cmd_ln_float32_r(m_pConfig, "-samprate"));

    if (m_pDevice == NULL) {
        throw std::runtime_error("skylight speech failed to open the default audio device");
    }
}

skylight::Speech::~Speech() {

    m_recording = false;

    if (m_recordingThread.joinable())
        m_recordingThread.join();

    if (ad_close(m_pDevice) < 0) {
        spdlog::error("skylight speech failed to close the default audio device");
    }

    if (ps_free(m_pSpeechDecoder) != 0) {
        spdlog::error("skylight speech failed to free the speech decoder");
    }

    if (cmd_ln_free_r(m_pConfig) != 0) {
        spdlog::error("skylight speech failed to free the command line arguments");
    }
}

void skylight::Speech::Start() {

    m_recording = true;
    m_recordingThread = std::thread(&Speech::RecordLoop, this);
}

std::string skylight::Speech::Stop() {

    m_recording = false;

    if (m_recordingThread.joinable())
        m_recordingThread.join();

    if (ps_start_utt(m_pSpeechDecoder) < 0) {
        spdlog::error("skylight speech failed to start utterance");
        return "";
    }

    {
        std::scoped_lock lock(m_bufferMutex);

        if (ps_process_raw(m_pSpeechDecoder, m_audioBuffer, m_audioBufferFront, false, true) < 0) {
            spdlog::error("skylight speech failed to process the raw audio");
            return "";
        }
    }

    if (ps_end_utt(m_pSpeechDecoder) < 0) {
        spdlog::error("skylight speech failed to stop utterance");
        return "";
    }

    int score;
    char const *command = ps_get_hyp(m_pSpeechDecoder, &score);
    if (command == NULL) {
        spdlog::warn("skylight speech failed to recognise any audio");
        return "";
    }

    return std::string(command);
}

bool skylight::Speech::SetJSGF(std::string jsgfFile) {

    spdlog::info("skylight speech setting new jsgf file {}", jsgfFile);
    static const arg_t cont_args_def[] = {POCKETSPHINX_OPTIONS,
                                          {"-adcdev", ARG_STRING, NULL, "Name of audio device to use for input."},
                                          CMDLN_EMPTY_OPTION};
    m_pConfig = cmd_ln_init(NULL, cont_args_def, FALSE, "-inmic", "yes", "-jsgf", jsgfFile.c_str(), NULL);
    ps_default_search_args(m_pConfig);

    if (m_pConfig == NULL) {
        spdlog::error("skylight speech failed to set config during JSGF refresh");
        return false;
    }

    if (ps_reinit(m_pSpeechDecoder, m_pConfig) != 0) {
        spdlog::error("skylight speech failed to re initialise the speech decoder after JSGF refresh");
        return false;
    }

    return true;
}


void skylight::Speech::RecordLoop() {

    if (ad_start_rec(m_pDevice) < 0) {
        spdlog::error("skylight speech failed to start recording");
        return;
    }

    //we assume that if we've started, we can stop and restart so these aren't checked.
    ad_stop_rec(m_pDevice);
    ad_start_rec(m_pDevice);

    {
        std::scoped_lock lock(m_bufferMutex);
        m_audioBufferFront = 0;
    }

    while (m_recording) {
        {
            std::scoped_lock lock(m_bufferMutex);
            int newSamples = 0;

            while (newSamples == 0) {
                newSamples = ad_read(m_pDevice, &m_audioBuffer[m_audioBufferFront], 16000);
            }

            if (newSamples < 0) {
                spdlog::error("skylight speech failed to read audio");
                return;
            }

            m_audioBufferFront += newSamples;

        }

        if (m_audioBufferFront + 16000 > m_sampleRate * m_maxRecordTime) {
            spdlog::warn("skylight speech has completely filled the audio buffer. Forcing recording stop");
            m_recording = false;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    
    if (ad_stop_rec(m_pDevice) < 0) {
        spdlog::error("skylight speech failed to stop recording");
        return;
    }

}
