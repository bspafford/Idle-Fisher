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

	// Adds new path to audio system queue
	void SetAudio(const std::string& path);

	// 1 is default, 2 doubles making it an octave higher, 0.5 halves it making it one octave lower
	void SetSpeed(float speed);

	void SetLoc(vector loc);

private:
	static inline std::recursive_mutex mutex;
	
	uint32_t audioId;
};