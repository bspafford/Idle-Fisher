#pragma once

#include <memory>
#include <string>
#include <mutex>

#include "vector.h"

#include <miniaudio.h>

enum class AudioType {
	Music,
	SFX,
	Ambient,
};

class Audio {
public:
	Audio(const std::string path, AudioType type);
	// puts the audio in world position
	Audio(const std::string path, AudioType type, vector loc);
	~Audio();

	void Play(bool loop = false);
	void Stop();

	std::string GetPath();
	void SetAudio(const std::string& path);

	// 1 is default, 2 doubles making it an octave higher, 0.5 halves it making it one octave lower
	void SetSpeed(float speed);

	ma_decoder* GetDecoder();
	ma_resampler* GetResampler();

	vector GetLoc();
	void SetLoc(vector loc);
	bool GetUseWorldPos();

	AudioType GetType();
	bool ShouldLoop();

	bool pendingStart = false;

	bool IsPlaying();

	// called when the audio first starts playing
	void Started();
	// called when the audio has finished playing
	void Finished();

	std::atomic<bool> deleted = false;

private:
	static inline std::recursive_mutex mutex;

	// audio stuff
	std::string path;
	ma_decoder* decoder;
	ma_resampler* resampler;
	AudioType type;

	// world stuff
	vector loc;
	bool useWorldPos = false;
	bool loop;

	bool isPlaying = false;
};