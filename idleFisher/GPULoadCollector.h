#pragma once

#include <mutex>

#include "Image.h"
#include "text.h"
#include "Texture.h"
#include "textureManager.h"

class GPULoadCollector {
public:
	static void open() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		imgToUpload.clear();
		textToUpload.clear();
		textureStructToUpload.clear();
		textureToUpload.clear();
		active = true;
	}

	static void setMainThread(std::thread::id mainThread) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		mainThreadId = mainThread;
	}

	static bool isOnMainThread() {
		return std::this_thread::get_id() == mainThreadId;
	}

	static void add(Image* img) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (active && !isOnMainThread())
			imgToUpload.push_back(img);
		else
			img->LoadGPU();
	}

	static void remove(Image* img) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		// list shouldn't contain anything
		if (!active)
			return;

		auto it = std::find(imgToUpload.begin(), imgToUpload.end(), img);
		if (it != imgToUpload.end())
			imgToUpload.erase(it);
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

	static void add(textureStruct* texture) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (active && !isOnMainThread())
			textureStructToUpload.push_back(texture);
		else
			texture->LoadGPU();
	}

	static void remove(textureStruct* texture) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		// list shouldn't contain anything
		if (!active)
			return;

		auto it = std::find(textureStructToUpload.begin(), textureStructToUpload.end(), texture);
		if (it != textureStructToUpload.end())
			textureStructToUpload.erase(it);
	}

	static void add(Texture* texture) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (active && !isOnMainThread())
			textureToUpload.push_back(texture);
		else
			texture->LoadGPU();
	}

	static void remove(Texture* texture) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		// list shouldn't contain anything
		if (!active)
			return;

		auto it = std::find(textureToUpload.begin(), textureToUpload.end(), texture);
		if (it != textureToUpload.end())
			textureToUpload.erase(it);
	}

	static void close() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		active = false;
	}

	static void LoadAllGPUData() {
		std::lock_guard<std::recursive_mutex> lock(mutex);

		for (textureStruct* t : textureStructToUpload)
			t->LoadGPU();
		textureStructToUpload.clear();

		for (Texture* t : textureToUpload)
			t->LoadGPU();
		textureToUpload.clear();

		for (Image* img : imgToUpload)
			img->LoadGPU();
		imgToUpload.clear();

		for (text* t : textToUpload)
			t->LoadGPU();
		textToUpload.clear();
	}

private:
	static inline std::vector<textureStruct*> textureStructToUpload;
	static inline std::vector<Texture*> textureToUpload;
	static inline std::vector<Image*> imgToUpload;
	static inline std::vector<text*> textToUpload;
	static inline bool active = false;
	static inline std::recursive_mutex mutex;
	static inline std::thread::id mainThreadId;
};