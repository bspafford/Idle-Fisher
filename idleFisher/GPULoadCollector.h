#pragma once

#include <iostream>
#include <mutex>

#include "text.h"

class GPULoadCollector {
public:
	static void open() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		textToUpload.clear();
		active = true;
	}

	static void setMainThread(std::thread::id mainThread) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		mainThreadId = mainThread;
	}

	static bool isOnMainThread() {
		return std::this_thread::get_id() == mainThreadId;
	}

	static void add(text* text) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (active && !isOnMainThread())
			textToUpload.push_back(text);
		else
			text->LoadGPU();
	}

	static void remove(text* text) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		// list shouldn't contain anything
		if (!active)
			return;

		auto it = std::find(textToUpload.begin(), textToUpload.end(), text);
		if (it != textToUpload.end())
			textToUpload.erase(it);
	}

	static void close() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		active = false;
	}

	static void LoadAllGPUData() {
		for (int i = 0; i < textToUpload.size(); i++) {
			textToUpload[i]->LoadGPU();
		}
		textToUpload.clear();
	}

private:
	static inline std::vector<text*> textToUpload;
	static inline bool active = false;
	static inline std::recursive_mutex mutex;
	static inline std::thread::id mainThreadId;
};