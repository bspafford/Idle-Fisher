#include "Audio.h"
#include "AudioSystem.h"

#include <iostream>

Audio::Audio(std::string path, AudioType type) {
	std::lock_guard lock(mutex);

	this->path = path;
	this->type = type;

	std::vector<uint8_t>* buffer = AudioSystem::GetAudioData(path);
	if (ma_decoder_init_memory(buffer->data(), buffer->size(), AudioSystem::GetDecoderConfig(), &decoder) != MA_SUCCESS) {
		std::cerr << "Failed to init decoder\n";
		return;
	}
}

Audio::Audio(std::string path, AudioType type, vector loc) {
	std::lock_guard lock(mutex);
	
	useWorldPos = true;
	this->loc = loc;

	this->path = path;
	this->type = type;

	std::vector<uint8_t>* buffer = AudioSystem::GetAudioData(path);
	if (ma_decoder_init_memory(buffer->data(), buffer->size(), AudioSystem::GetDecoderConfig(), &decoder) != MA_SUCCESS) {
		std::cerr << "Failed to init decoder\n";
		return;
	}
}

Audio::~Audio() {
	ma_decoder_uninit(&decoder);
}

void Audio::Play(bool loop) {
	this->loop = loop;
	pendingStart = true;
	AudioSystem::Add(this);
}

void Audio::Stop() {
	AudioSystem::Remove(this);
}

void Audio::SetPitch(float pitch) {
	//ma_sound_set_pitch(&sound, pitch);
}

ma_decoder* Audio::GetDecoder() {
	return &decoder;
}

vector Audio::GetLoc() {
	return loc;
}

bool Audio::GetUseWorldPos() {
	return useWorldPos;
}

AudioType Audio::GetType() {
	return type;
}

bool Audio::ShouldLoop() {
	return loop;
}