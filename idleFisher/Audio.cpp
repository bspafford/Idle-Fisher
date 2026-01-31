#include "Audio.h"
#include "PakReader.h"

#include <iostream>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

void Audio::Init() {
    std::lock_guard lock(mutex);

    if (ma_engine_init(nullptr, &engine) != MA_SUCCESS) {
        std::cerr << "Failed to init engine\n";
        return;
    }

    PakReader::ParseAudio("data/audio.pak");
}

void Audio::LoadData() {
    PakReader::LoadAllAudio(audioMap);
    isAudioLoaded = true;
}

void Audio::Shutdown() {
    ma_engine_uninit(&engine);
}

Audio::Audio(std::string path) {
    std::lock_guard lock(mutex);

    this->path = path;

    std::vector<uint8_t>* buffer = GetAudioData(path);
    if (ma_decoder_init_memory(buffer->data(), buffer->size(), nullptr, &decoder) != MA_SUCCESS) {
        std::cerr << "Failed to init decoder\n";
        return;
    }

    if (ma_sound_init_from_data_source(&engine, &decoder, 0, nullptr, &sound) != MA_SUCCESS) {
        std::cerr << "Failed to init sound\n";
    }
}

Audio::~Audio() {
    ma_sound_uninit(&sound);
    ma_decoder_uninit(&decoder);
}

void Audio::Play() {
    ma_sound_start(&sound);
}

void Audio::Stop() {
    ma_sound_stop(&sound);
}

void Audio::SetPitch(float pitch) {
    ma_sound_set_pitch(&sound, pitch);
}

std::vector<uint8_t>* Audio::GetAudioData(const std::string& path) {
    std::lock_guard lock(mutex);

    uint32_t hashedId = PakReader::Hash(path);
    auto it = audioMap.find(hashedId);
    if (it != audioMap.end()) {
        return it->second.get();
    } else if (!isAudioLoaded) {
        auto [it, inserted ] = audioMap.emplace(hashedId, std::make_unique<std::vector<uint8_t>>(PakReader::LoadAudio(path)));
        return it->second.get();
    } else {
        std::cerr << "Audio file not in audioMap: \"" << path << "\"\n";
        abort();
        return nullptr;
    }
}