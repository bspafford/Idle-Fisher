#include "Audio.h"
#include "AudioSystem.h"

#include <iostream>

#include "debugger.h"

Audio::Audio(std::string path, AudioType type) {
	std::lock_guard lock(mutex);

	audioId = AudioSystem::CreateAudioObject(path, type, false);
}

Audio::Audio(std::string path, AudioType type, vector loc) {
	std::lock_guard lock(mutex);

	audioId = AudioSystem::CreateAudioObject(path, type, true, loc);
}

Audio::~Audio() {
	AudioSystem::QueueCommand({ AudioCmdType::Destroy, audioId, 0.0f });
}

void Audio::Play(bool loop) {
	AudioSystem::QueueCommand({ AudioCmdType::Play, audioId, static_cast<float>(loop) });
}

void Audio::Stop() {
	AudioSystem::QueueCommand({ AudioCmdType::Stop, audioId, 0.0f });
}

bool Audio::IsPlaying() {
	return false;
	//return AudioSystem::IsPlaying(audioId);
}

void Audio::SetAudio(const std::string& path) {
	AudioSystem::QueueCommand({ AudioCmdType::SetAudio, audioId, 0.0f, path });
}

void Audio::SetSpeed(float speed) {
	AudioSystem::SetSpeed(audioId, speed);
}

vector Audio::GetLoc() {
	return vector(0, 0);
	//return loc;
}

void Audio::SetLoc(vector loc) {

	//this->loc = loc;
}

bool Audio::GetUseWorldPos() {
	return false;
	//return useWorldPos;
}

AudioType Audio::GetType() {
	return AudioType::SFX;
	//return type;
}

bool Audio::ShouldLoop() {
	return false;
	//return loop;
}