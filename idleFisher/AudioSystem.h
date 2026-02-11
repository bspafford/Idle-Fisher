#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <unordered_set>
#include <miniaudio.h>

class Audio;

class AudioSystem {
public:
	// Loads the engine and the initial load for the pak file, done on main thread
	static void Init();
	// load the body of the audio files, called during first load
	static void LoadData();
	static void Shutdown();

	static ma_device* GetDevice();
	static ma_decoder_config* GetDecoderConfig();
	static std::vector<uint8_t>* GetAudioData(const std::string& path);

	static void Add(Audio* audio);
	static void Remove(Audio* audio);
	static void ChangeAudio(Audio* audio, const std::string& path);

private:
	static void Mix(float* out, ma_uint32 frames);

	static void DataCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);
	static void ProcessAudio(float* out, ma_uint32 frames);

	static inline std::unordered_map<uint32_t, std::unique_ptr<std::vector<uint8_t>>> audioMap;
	static inline bool isAudioLoaded = false;

	static inline ma_device device;
	static inline ma_device_config deviceConfig;
	static inline ma_decoder_config decoderConfig;
	static inline float phase = 0.0f;

	static void ApplyPendingChanges();
	static inline std::unordered_set<Audio*> audioList;
	static inline std::unordered_set<Audio*> pendingAdd;
	static inline std::unordered_set<Audio*> pendingRemove;
	static inline std::unordered_map<Audio*, std::string> pendingChange;
};