#include "AudioSystem.h"
#include "PakReader.h"
#include "Audio.h"
#include "saveData.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

void AudioSystem::Init() {
	ma_device_config config = ma_device_config_init(ma_device_type_playback);

	config.playback.format = ma_format_f32;
	config.playback.channels = 2;
	config.sampleRate = 48000;

	config.dataCallback = AudioSystem::DataCallback;
	config.pUserData = nullptr;

	ma_result result = ma_device_init(nullptr, &config, &device);
	if (result != MA_SUCCESS) {
		std::cerr << "Failed to init device: " << result << "\n";
		return;
	}

	decoderConfig = ma_decoder_config_init(config.playback.format, config.playback.channels, config.sampleRate);

	ma_device_start(&device);

	PakReader::ParseAudio("data/audio.pak");
}

void AudioSystem::LoadData() {
	PakReader::LoadAllAudio(audioMap);
	isAudioLoaded = true;
}

void AudioSystem::Shutdown() {
	ma_device_uninit(&device);
}

void AudioSystem::Mix(float* out, ma_uint32 frameCount) {
	memset(out, 0, frameCount * 2 * sizeof(float)); // stereo

	ApplyPendingChanges();

	float dRef = 10;
	for (Audio* audio : audioList) {
		if (audio->pendingStart) { // set back to beginning
			ma_decoder_seek_to_pcm_frame(audio->GetDecoder(), 0);
			audio->pendingStart = false;
		}
		
		std::vector<float> temp(frameCount * audio->GetDecoder()->outputChannels);
		ma_uint64 framesRead = 0;
		ma_result r = ma_decoder_read_pcm_frames(audio->GetDecoder(), temp.data(), frameCount, &framesRead);

		float volume = SaveData::settingsData.masterVolume / 100.f;
		if (audio->GetType() == AudioType::Music)
			volume *= SaveData::settingsData.musicVolume / 100.f;
		else if (audio->GetType() == AudioType::SFX)
			volume *= SaveData::settingsData.sfxVolume / 100.f;
		
		if (audio->GetUseWorldPos()) {
			float distance = math::max(math::distance(SaveData::saveData.playerLoc, audio->GetLoc()), 1.f); // make sure volume doesn't get too lound
			volume *= dRef / distance;
			vector direction = math::normalize(audio->GetLoc() - SaveData::saveData.playerLoc);

			float leftDirVolume = math::dot(vector(-1, 0), direction) * 0.175f + 0.825f; // [0.65, 1]
			float rightDirVolume = math::dot(vector(1, 0), direction) * 0.175f + 0.825f; // [0.65, 1]
			for (ma_uint32 i = 0; i < framesRead * 2; i++) {
				if (i % 2 == 0) { // left
					out[i] += temp[i] * volume * leftDirVolume;
				} else { // right
					out[i] += temp[i] * volume * rightDirVolume;
				}
			}

		} else {
			// simple stereo mix
			if (audio->GetDecoder()->outputChannels == 1) {
				for (ma_uint32 i = 0; i < framesRead; i++) {
					out[i * 2 + 0] += temp[i] * volume;
					out[i * 2 + 1] += temp[i] * volume;
				}
			} else {
				for (ma_uint32 i = 0; i < framesRead * 2; i++) {
					out[i] += temp[i] * volume;
				}
			}
		}

		// loop or remove finished audio
		if (r == MA_AT_END) {
			if (audio->ShouldLoop())
				ma_decoder_seek_to_pcm_frame(audio->GetDecoder(), 0);
			else
				audio->Stop();
				//audio->
		}
	}
}

void AudioSystem::DataCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
	AudioSystem::Mix((float*)output, frameCount);
}

std::vector<uint8_t>* AudioSystem::GetAudioData(const std::string& path) {
	uint32_t hashedId = PakReader::Hash(path);
	auto it = audioMap.find(hashedId);
	if (it != audioMap.end()) {
		return it->second.get();
	} else if (!isAudioLoaded) {
		auto [it, inserted] = audioMap.emplace(hashedId, std::make_unique<std::vector<uint8_t>>(PakReader::LoadAudio(path)));
		return it->second.get();
	} else {
		std::cerr << "Audio file not in audioMap: \"" << path << "\"\n";
		abort();
		return nullptr;
	}
}

void AudioSystem::ApplyPendingChanges() {
	for (Audio* audio : pendingAdd) {
		audioList.push_back(audio);
	}

	for (Audio* audio : pendingRemove) {
		auto it = std::find(audioList.begin(), audioList.end(), audio);
		if (it != audioList.end()) audioList.erase(it);
	}

	pendingAdd.clear();
	pendingRemove.clear();
}

void AudioSystem::Add(Audio* audio) {
	pendingAdd.push_back(audio);
}

void AudioSystem::Remove(Audio* audio) {
	pendingRemove.push_back(audio);
}

ma_device* AudioSystem::GetDevice() {
	return &device;
}

ma_decoder_config* AudioSystem::GetDecoderConfig() {
	return &decoderConfig;
}