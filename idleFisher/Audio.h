#pragma once

#include <miniaudio.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

class Audio {
public:
	// Loads the engine and the initial load for the pak file, done on main thread
	static void Init();
	// load the body of the audio files, called during first load
	static void LoadData();
	static void Shutdown();
	
	Audio(const std::string path);
	~Audio();

	void Play();
	void Stop();

	// 1 is default, 2 doubles making it an octave higher, 0.5 halves it making it one octave lower
	void SetPitch(float pitch);

private:
	static inline std::recursive_mutex mutex;
	static inline ma_engine engine;
	static inline std::unordered_map<uint32_t, std::unique_ptr<std::vector<uint8_t>>> audioMap;
	static inline bool isAudioLoaded = false;

	std::vector<uint8_t>* GetAudioData(const std::string& path);

	ma_sound sound;
	ma_decoder decoder;
	std::string path;
	std::vector<uint8_t>* buffer;
};