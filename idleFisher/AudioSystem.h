#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <miniaudio.h>
#include <atomic>
#include <mutex>

#include "AudioObject.h"

enum class AudioType;

struct AudioState {
	std::atomic<bool> playing = false;
	std::atomic<bool> looping = false;
};

struct Slot {
	uint32_t id = 0u; // lifetime id
	uint16_t generation = 1u; // play id generation
	AudioState state;
};

enum class AudioCmdType {
	Play,
	Stop,
	Destroy,
	SetAudio,
	SetSpeed,
	SetLoc,
	Create,
};

struct AudioCommand {
	AudioCmdType type;
	uint32_t id;
	float fltValue; // optional for some commands
	std::string strValue; // optional for some commands
	vector loc; // optional for some commands
};

template <typename T, size_t N>
class RingBuffer {
public:
	RingBuffer() : head(0), tail(0) {}

	bool push(const T& value) {
		auto next = (head + 1) % N;
		if (next == tail.load(std::memory_order_acquire)) {
			return false; // full
		}
		buffer[head] = value;
		head = next;
		return true;
	}

	bool pop(T& out) {
		if (tail.load(std::memory_order_acquire) == head) {
			return false; // empty
		}
		out = buffer[tail];
		tail.store((tail + 1) % N, std::memory_order_release);
		return true;
	}

private:
	T buffer[N];
	size_t head;
	std::atomic<size_t> tail;
};

class AudioSystem {
public:
	// Loads the engine and the initial load for the pak file, done on main thread
	static void Init();
	// load the body of the audio files, called during first load
	static void LoadData();
	static void Shutdown();

	static ma_decoder_config* GetDecoderConfig();
	static std::vector<uint8_t>* GetAudioData(const std::string& path);

	static uint32_t CreateAudioObject(std::string path, AudioType type, bool useWorldSpace, vector loc = vector(0, 0));

	static void QueueCommand(const AudioCommand&);

	static inline RingBuffer<AudioCommand, 256> commandQueue;
private:
	static void DataCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);
	static void Mix(float* out, ma_uint32 frames);

	static bool IsValid(uint32_t id);
	static uint16_t GetIndex(uint32_t id);
	static uint16_t GetGeneration(uint32_t id);
	static uint32_t GetPlayIdFrom(uint16_t generation, uint16_t index);
	static void Destroy(uint32_t id);

	static inline std::unordered_map<uint32_t, std::unique_ptr<std::vector<uint8_t>>> audioMap;
	static inline bool isAudioLoaded = false;

	static inline ma_device device;
	static inline ma_device_config deviceConfig;
	static inline ma_decoder_config decoderConfig;
	static inline float phase = 0.0f;

	static void ApplyPendingChanges();
	static void HandleCommand(const AudioCommand& cmd);

	// lifetime IDs
	static inline uint32_t idNum;
	static uint32_t TakeID();
	static inline std::unordered_map<uint32_t, std::unique_ptr<AudioObject>> audioInstances;
	static inline std::vector<uint32_t> freeIDs;

	// play time IDs
	static inline const int maxSlots = 128;
	static inline std::array<Slot, maxSlots> slots; // only active currently playing audio
	static inline std::vector<uint16_t> freeIndices;

	static inline std::mutex mutex;
};