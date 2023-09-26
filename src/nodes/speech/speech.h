#pragma once

#include <chrono>
#include <sys/select.h>
#include <atomic>
#include <optional>
#include <thread>
#include <sphinxbase/ad.h>
#include <pocketsphinx/pocketsphinx.h>
#include <mutex>
#include <functional>
#include <filesystem>

namespace skylight {

    class Speech {
    public:
        Speech(std::string audioDevice, std::filesystem::path jsgfFile = "");

        ~Speech();

        void Start();

        std::string Stop();

        bool SetJSGF(std::filesystem::path jsgfFile);

    private:

        void RecordLoop();

        const static int m_sampleRate = 16000;
        const static int m_maxRecordTime = 5; //seconds

        cmd_ln_t *m_pConfig;
        ps_decoder_t *m_pSpeechDecoder;
        ad_rec_t *m_pDevice;

        std::atomic_bool m_recording = false;
        std::thread m_recordingThread;

        std::mutex m_bufferMutex;
        short m_audioBuffer[m_sampleRate * m_maxRecordTime];
        int m_audioBufferFront;

    };
}
