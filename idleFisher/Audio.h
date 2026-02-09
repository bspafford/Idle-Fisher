#pragma once

#include <memory>
#include <string>
#include <vector>
#include <mutex>

#include "vector.h"

#include <miniaudio.h>

enum class AudioType {
	Music,
	SFX,
};

class Audio {
public:
	Audio(const std::string path, AudioType type);
	// puts the audio in world position
	Audio(const std::string path, AudioType type, vector loc);
	~Audio();

	void Play(bool loop = false);
	void Stop();

	// 1 is default, 2 doubles making it an octave higher, 0.5 halves it making it one octave lower
	void SetPitch(float pitch);

	ma_decoder* GetDecoder();

	vector GetLoc();
	bool GetUseWorldPos();

	AudioType GetType();
	bool ShouldLoop();

	bool pendingStart = false;

private:
	static inline std::recursive_mutex mutex;

	// audio stuff
	std::string path;
	ma_decoder decoder;
	AudioType type;

	// world stuff
	vector loc;
	bool useWorldPos = false;
	bool loop;
};