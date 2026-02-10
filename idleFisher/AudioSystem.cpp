#include "AudioSystem.h"
#include "PakReader.h"
#include "Audio.h"
#include "saveData.h"
#include "character.h"

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

void AudioSystem::Mix(float* out, ma_uint32 frameCount32) {
	ma_uint64 frameCount = frameCount32;
	memset(out, 0, frameCount * 2 * sizeof(float)); // stereo

	ApplyPendingChanges();

	Acharacter* character = GetCharacter();
	vector charLoc = vector(0, 0);
	if (character)
		charLoc = character->getCharLoc();

	for (Audio* audio : audioList) {
		if (audio->pendingStart) { // set back to beginning
			ma_decoder_seek_to_pcm_frame(audio->GetDecoder(), 0);
			ma_resampler_reset(audio->GetResampler());
			audio->pendingStart = false;
			audio->Started();
		}

		ma_uint64 inFramesNeeded;
		ma_resampler_get_required_input_frame_count(audio->GetResampler(), frameCount, &inFramesNeeded);

		std::vector<float> pFramesIn(inFramesNeeded * audio->GetDecoder()->outputChannels);
		std::vector<float> pFramesOut(frameCount * audio->GetDecoder()->outputChannels);

		ma_uint64 framesRead = 0;
		// Read from decoder (source frames)
		ma_decoder_read_pcm_frames(audio->GetDecoder(), pFramesIn.data(), inFramesNeeded, &framesRead);

		// loop or remove finished audio
		if (framesRead < inFramesNeeded) { // audio has finished
			ma_decoder_seek_to_pcm_frame(audio->GetDecoder(), 0);
			ma_resampler_reset(audio->GetResampler());
			if (audio->ShouldLoop()) {
				ma_uint64 moreRead = 0;
				ma_decoder_read_pcm_frames(
					audio->GetDecoder(),
					pFramesIn.data() + framesRead * audio->GetDecoder()->outputChannels,
					inFramesNeeded - framesRead,
					&moreRead
				);

				framesRead += moreRead;
			} else { // don't loop
				audio->Finished();
				audio->Stop();
				continue;
			}
		}

		ma_uint64 framesOut = frameCount;
		ma_resampler_process_pcm_frames(audio->GetResampler(), pFramesIn.data(), &framesRead, pFramesOut.data(), &framesOut);

		float volume = SaveData::settingsData.masterVolume / 100.f;
		if (audio->GetType() == AudioType::Music)
			volume *= SaveData::settingsData.musicVolume / 100.f;
		else if (audio->GetType() == AudioType::SFX)
			volume *= SaveData::settingsData.sfxVolume / 100.f;
		else if (audio->GetType() == AudioType::Ambient)
			volume *= SaveData::settingsData.ambientVolume / 100.f;

		if (audio->GetUseWorldPos()) {
			float distance = math::distance(charLoc, audio->GetLoc());
			volume *= 30.f / distance;
			vector direction = math::normalize(audio->GetLoc() - SaveData::saveData.playerLoc);

			float leftDirVolume = math::dot(vector(-1, 0), direction) * 0.175f + 0.825f; // [0.65, 1]
			float rightDirVolume = math::dot(vector(1, 0), direction) * 0.175f + 0.825f; // [0.65, 1]
			for (ma_uint32 i = 0; i < framesOut * 2; i++) {
				if (i % 2 == 0) { // left
					out[i] += pFramesOut[i] * volume * leftDirVolume;
				} else { // right
					out[i] += pFramesOut[i] * volume * rightDirVolume;
				}
			}

		} else {
			// simple stereo mix
			if (audio->GetDecoder()->outputChannels == 1) {
				for (ma_uint32 i = 0; i < framesOut; i++) {
					out[i * 2 + 0] += pFramesOut[i] * volume;
					out[i * 2 + 1] += pFramesOut[i] * volume;
				}
			} else {
				for (ma_uint32 i = 0; i < framesOut * 2; i++) {
					out[i] += pFramesOut[i] * volume;
				}
			}
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
	for (Audio* audio : pendingAdd)
		audioList.insert(audio);

	for (Audio* audio : pendingRemove)
		audioList.erase(audio);

	pendingAdd.clear();
	pendingRemove.clear();
}

void AudioSystem::Add(Audio* audio) {
	pendingAdd.insert(audio);
}

void AudioSystem::Remove(Audio* audio) {
	pendingRemove.insert(audio);
}

ma_device* AudioSystem::GetDevice() {
	return &device;
}

ma_decoder_config* AudioSystem::GetDecoderConfig() {
	return &decoderConfig;
}