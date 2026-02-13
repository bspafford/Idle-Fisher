#pragma once

#include <string>
#include <miniaudio.h>
#include <atomic>
#include <iostream>

#include "math.h"

enum class AudioType;

class AudioObject {
	friend class Audio;
	friend class AudioSystem;

public:
	AudioObject(uint32_t id, std::string path, AudioType type, bool useWorldPos, vector loc);
	~AudioObject();

private:
	// audio system calls to update decoder/resampler
	void AudioSystemSetAudio(const std::string& path);

	// called when the audio first starts playing
	void Started(uint32_t index);
	// called when the audio has finished playing
	void Finished();

	// audio stuff
	uint32_t id;
	std::string path;
	ma_decoder* decoder;
	ma_resampler* resampler;
	AudioType type;

	// world stuff
	vector loc;
	bool useWorldPos = false;

	uint32_t playId;
};