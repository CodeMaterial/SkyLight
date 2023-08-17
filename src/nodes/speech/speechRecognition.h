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
#include <tef/aurora/runnable.h>

namespace TEF::Aurora {
	class SpeechRecognition : public Runnable
	{
	public:
		SpeechRecognition() = default;
		~SpeechRecognition();

		bool Connect(std::string audioDevice);

		bool Start();
		bool Stop();

		bool SetRecordFile(std::string filepath);

		bool SetJSGF(std::string jsgfFile);

		void RegisterCommandCallback(std::function<void(std::string command)> cb) { m_commandCallback = cb; };

	private:

		bool RecordLoop();

		void SaveBuffer(std::string filepath);

		const static int m_sampleRate = 16000;
		const static int m_maxRecordTime = 10; //seconds

		cmd_ln_t* m_pConfig;
		ps_decoder_t* m_pSpeechDecoder;
		ad_rec_t* m_pDevice;

		std::atomic_bool m_recording = false;
		std::thread m_recordingThread;

		std::mutex m_bufferMutex;
		short m_audioBuffer[m_sampleRate * m_maxRecordTime];
		int m_audioBufferFront;

		std::function<void(std::string command)> m_commandCallback;

		std::optional<std::string> m_audioFilepathDebug;
	};
}