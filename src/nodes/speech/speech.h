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

namespace skylight {

    class Speech {
    public:
        Speech();

        ~Speech();

        bool Connect(std::string audioDevice);

        bool Start();

        std::string Stop();

        //bool GenerateJSGF(std::string jsgfFile);

        //bool SetJSGF(std::string jsgfFile);

    private:

        bool RecordLoop();

        const static int m_sampleRate = 16000;
        const static int m_maxRecordTime = 10; //seconds

        cmd_ln_t *m_pConfig;
        ps_decoder_t *m_pSpeechDecoder;
        ad_rec_t *m_pDevice;

        std::atomic_bool m_recording = false;
        std::thread m_recordingThread;

        std::mutex m_bufferMutex;
        short m_audioBuffer[m_sampleRate * m_maxRecordTime];
        int m_audioBufferFront;

        bool m_connected = false;

    };
}
