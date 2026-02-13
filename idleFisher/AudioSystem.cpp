#include "AudioSystem.h"
#include "PakReader.h"
#include "Audio.h"
#include "saveData.h"
#include "character.h"
#include "main.h"
#include "AudioObject.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

void AudioSystem::Init() {
	freeIndices.resize(maxSlots);
	for (int i = 0; i < maxSlots; i++)
		freeIndices[i] = i;

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
	ma_device_stop(&device);
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

	for (Slot& slot : slots) {
		if (!slot.state.playing.load())
			continue;

		AudioObject* audio = audioInstances.at(slot.id).get();

		ma_uint64 inFramesNeeded;
		ma_resampler_get_required_input_frame_count(audio->resampler, frameCount, &inFramesNeeded);

		std::vector<float> pFramesIn(inFramesNeeded * audio->decoder->outputChannels);
		std::vector<float> pFramesOut(frameCount * audio->decoder->outputChannels);

		ma_uint64 framesRead = 0;
		// Read from decoder (source frames)
		ma_decoder_read_pcm_frames(audio->decoder, pFramesIn.data(), inFramesNeeded, &framesRead);

		// loop or remove finished audio
		if (framesRead < inFramesNeeded) { // audio has finished
			ma_decoder_seek_to_pcm_frame(audio->decoder, 0);
			ma_resampler_reset(audio->resampler);
			if (audio->loop) {
				ma_uint64 moreRead = 0;
				ma_decoder_read_pcm_frames(
					audio->decoder,
					pFramesIn.data() + framesRead * audio->decoder->outputChannels,
					inFramesNeeded - framesRead,
					&moreRead
				);

				framesRead += moreRead;
			} else { // don't loop
				audio->Finished();
				QueueCommand({ AudioCmdType::Stop, audio->id });
				continue;
			}
		}

		ma_uint64 framesOut = frameCount;
		ma_resampler_process_pcm_frames(audio->resampler, pFramesIn.data(), &framesRead, pFramesOut.data(), &framesOut);

		float volume = SaveData::settingsData.masterVolume / 100.f;
		if (audio->type == AudioType::Music)
			volume *= SaveData::settingsData.musicVolume / 100.f;
		else if (audio->type == AudioType::SFX)
			volume *= SaveData::settingsData.sfxVolume / 100.f;
		else if (audio->type == AudioType::Ambient)
			volume *= SaveData::settingsData.ambientVolume / 100.f;

		if (audio->useWorldPos) {
			float distance = math::distance(charLoc, audio->loc);
			volume *= 30.f / distance;
			vector direction = math::normalize(audio->loc - SaveData::saveData.playerLoc);

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
			if (audio->decoder->outputChannels == 1) {
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
	AudioCommand cmd;
	while (commandQueue.pop(cmd)) {
		HandleCommand(cmd);
	}
}

void AudioSystem::QueueCommand(const AudioCommand& cmd) {
	commandQueue.push(cmd);
}

void AudioSystem::HandleCommand(const AudioCommand& cmd) {
	std::cout << "running command: " << static_cast<int>(cmd.type) << "\n";

	switch (cmd.type) {
	case AudioCmdType::Play: {
		uint16_t index;
		Slot* slot = nullptr;
		// what if i play then play again while still playing, its going to take up 2 slots. I need to see if its playing, then 
		AudioObject* audioObject = audioInstances.at(cmd.id).get();
		if (IsValid(audioObject->playId)) {
			index = GetIndex(audioObject->playId);
			slot = &slots[index];
		} else if (!freeIndices.empty()) { // if there is a free indice
			// take index, and remove from free list
			index = freeIndices.back();
			freeIndices.pop_back();

			slot = &slots[index];
			slot->id = cmd.id;
			slot->generation++; // invalidate old ids
		} else { // if there is no free slot
			std::cout << "no free slot\n";
			assert(false);
		}

		slot->state.playing = true;
		slot->state.looping = cmd.fltValue; // should loop

		// seek back to start
		ma_decoder_seek_to_pcm_frame(audioObject->decoder, 0);
		ma_resampler_reset(audioObject->resampler);
		audioObject->Started(GetPlayIdFrom(slot->generation, index));
		break;
	} case AudioCmdType::Stop: {
		AudioObject* audioObject = audioInstances.at(cmd.id).get();

		if (!IsValid(audioObject->playId))
			break; // not valid
		uint16_t index = GetIndex(audioObject->playId);
		Slot& slot = slots[index];
		slot.state.playing.store(false); // no longer playing
		audioObject->playId = 0u; // set 0, no longer playing

		freeIndices.push_back(index); // add back to free indices

		break;
	} case AudioCmdType::Destroy: {
		Destroy(cmd.id);
		break;
	} case AudioCmdType::SetAudio: {
		AudioObject* audioObject = audioInstances.at(cmd.id).get();
		audioObject->AudioSystemSetAudio(cmd.strValue);
		break;
	}
	}
}

ma_decoder_config* AudioSystem::GetDecoderConfig() {
	return &decoderConfig;
}

bool AudioSystem::IsValid(uint32_t id) {
	uint16_t index = GetIndex(id);
	if (index >= slots.size())
		return false;

	return slots[index].generation == GetGeneration(id);
}

uint32_t AudioSystem::CreateAudioObject(std::string path, AudioType type, bool useWorldSpace, vector loc) {
	uint32_t id = TakeID();
	audioInstances[id] = std::make_unique<AudioObject>(id, path, type, useWorldSpace, loc);
	return id;
}

uint32_t AudioSystem::TakeID() {
	uint32_t id = 0u;
	if (!freeIDs.empty()) { // there are free IDs
		id = freeIDs.back();
		freeIDs.pop_back();
	} else { // there wasn't a free id
		// look for heighest index
		for (auto& [audioId, audioObject] : audioInstances) {
			if (id < audioId)
				id = audioId;
		}
		++id;
	}
	return id;
}

void AudioSystem::Destroy(uint32_t id) {
	auto it = audioInstances.find(id);
	if (it == audioInstances.end())
		return;

	if (IsValid(it->second->playId)) // make sure the object is valid, before making play false
		slots[GetIndex(it->second->playId)].state.playing.store(false);

	audioInstances.erase(it);
	freeIDs.push_back(id);
}

/*AudioSystem::Get(uint32_t id) {
	uint16_t index = GetIndex(id);
	uint16_t gen = GetGeneration(id);

	if (index >= slots.size())
		return nullptr;

	Slot& slot = slots[index];

	if (slot.generation != gen)
		return nullptr;

	return slot.object.get();
}*/

uint16_t AudioSystem::GetIndex(uint32_t id) {
	return (uint16_t)id;
}

uint16_t AudioSystem::GetGeneration(uint32_t id) {
	return uint16_t(id >> 16);
}

uint32_t AudioSystem::GetPlayIdFrom(uint16_t generation, uint16_t index) {
	return (uint32_t(generation) << 16) | uint32_t(index);
}

bool AudioSystem::IsPlaying(uint32_t id) {
	return false;
	//return audioInstances.at(id)->isPlaying;
}

void AudioSystem::SetSpeed(uint32_t id, float speed) {
	
	//ma_uint32 baseRate = decoder->outputSampleRate;

	//ma_uint32 inRate = baseRate;
	//ma_uint32 outRate = (ma_uint32)(baseRate * speed + 0.5f);

	//ma_resampler_set_rate(resampler, inRate, outRate);
}