#pragma once

#include <memory>
#include <string>
#include <vector>
#include <mutex>

#include "vector.h"

#include <miniaudio.h>

class Audio {
public:
	Audio(const std::string path);
	// puts the audio in world position
	Audio(const std::string path, vector loc);
	~Audio();

	void Play();
	void Stop();

	// 1 is default, 2 doubles making it an octave higher, 0.5 halves it making it one octave lower
	void SetPitch(float pitch);

	ma_decoder* GetDecoder();

	vector GetLoc();
	bool GetUseWorldPos();

	bool pendingStart = false;

private:
	static inline std::recursive_mutex mutex;

	// audio stuff
	std::string path;
	ma_decoder decoder;

	// world stuff
	vector loc;
	bool useWorldPos = false;
};