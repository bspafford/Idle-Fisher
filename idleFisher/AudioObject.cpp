#include "AudioObject.h"
#include "AudioSystem.h"

AudioObject::AudioObject(uint32_t id, std::string path, AudioType type, bool useWorldPos, vector loc) {
	this->id = id;
	this->useWorldPos = useWorldPos;
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

AudioObject::~AudioObject() {
	ma_resampler_uninit(resampler, nullptr);
	ma_decoder_uninit(decoder);

	delete decoder;
	delete resampler;

	decoder = nullptr;
	resampler = nullptr;
}

void AudioObject::AudioSystemSetAudio(const std::string& path) {
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

void AudioObject::Started(uint32_t playId) {
	this->playId = playId;
	isPlaying = true;
}

void AudioObject::Finished() {
	isPlaying = false;
}