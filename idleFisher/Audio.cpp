#include "Audio.h"
#include "AudioSystem.h"

#include <iostream>

#include "debugger.h"

Audio::Audio(std::string path, AudioType type) {
	std::lock_guard lock(mutex);

	this->path = path;
	this->type = type;

	std::vector<uint8_t>* buffer = AudioSystem::GetAudioData(path);
	decoder = new ma_decoder();
	if (ma_decoder_init_memory(buffer->data(), buffer->size(), AudioSystem::GetDecoderConfig(), decoder) != MA_SUCCESS) {
		std::cerr << "Failed to init decoder\n";
		return;
	}

	resampler = new ma_resampler();
	ma_resampler_config resamplerConfig = ma_resampler_config_init(decoder->outputFormat, decoder->outputChannels, decoder->outputSampleRate, decoder->outputSampleRate, ma_resample_algorithm_linear); // start at 1.0x
	if (ma_resampler_init(&resamplerConfig, nullptr, resampler) != MA_SUCCESS) {
		std::cerr << "Failed to init resampler\n";
		return;
	}
}

Audio::Audio(std::string path, AudioType type, vector loc) {
	std::lock_guard lock(mutex);
	
	useWorldPos = true;
	this->loc = loc;

	this->path = path;
	this->type = type;

	decoder = new ma_decoder();
	std::vector<uint8_t>* buffer = AudioSystem::GetAudioData(path);
	if (ma_decoder_init_memory(buffer->data(), buffer->size(), AudioSystem::GetDecoderConfig(), decoder) != MA_SUCCESS) {
		std::cerr << "Failed to init decoder\n";
		return;
	}

	resampler = new ma_resampler();
	ma_resampler_config resamplerConfig = ma_resampler_config_init(decoder->outputFormat, decoder->outputChannels, decoder->outputSampleRate, decoder->outputSampleRate, ma_resample_algorithm_linear); // start at 1.0x
	if (ma_resampler_init(&resamplerConfig, nullptr, resampler) != MA_SUCCESS) {
		std::cerr << "Failed to init resampler\n";
		return;
	}
}

Audio::~Audio() {
	ma_resampler_uninit(resampler, nullptr);
	ma_decoder_uninit(decoder);

	delete decoder;
	delete resampler;

	decoder = nullptr;
	resampler = nullptr;
	pendingStart = false;
	deleted.store(true);
}

void Audio::Play(bool loop) {
	this->loop = loop;
	pendingStart = true;
	AudioSystem::Add(this);
}

void Audio::Stop() {
	AudioSystem::Remove(this);
}

bool Audio::IsPlaying() {
	return isPlaying;
}

void Audio::Started() {
	isPlaying = true;
}

void Audio::Finished() {
	isPlaying = false;
}

std::string Audio::GetPath() {
	return path;
}

void Audio::SetAudio(const std::string& path) {
	if (this->path == path) // return if same path
		return;

	this->path = path;

	std::vector<uint8_t>* buffer = AudioSystem::GetAudioData(path);
	ma_decoder_uninit(decoder);
	if (ma_decoder_init_memory(buffer->data(), buffer->size(), AudioSystem::GetDecoderConfig(), decoder) != MA_SUCCESS) {
		std::cerr << "Failed to init decoder\n";
		return;
	}
}

void Audio::SetSpeed(float speed) {
	ma_uint32 baseRate = decoder->outputSampleRate;

	ma_uint32 inRate = baseRate;
	ma_uint32 outRate = (ma_uint32)(baseRate * speed + 0.5f);

	ma_resampler_set_rate(resampler, inRate, outRate);
}

ma_decoder* Audio::GetDecoder() {
	return decoder;
}

ma_resampler* Audio::GetResampler() {
	return resampler;
}

vector Audio::GetLoc() {
	return loc;
}

void Audio::SetLoc(vector loc) {
	this->loc = loc;
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