#pragma once

#include <iostream>
#include <mutex>

#include "Image.h"
#include "animation.h"
#include "text.h"
#include "Rectangle.h"

class GPULoadCollector {
public:
	static void open() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		imagesToUpload.clear();
		animationsToUpload.clear();
		textToUpload.clear();
		rectToUpload.clear();
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
			imagesToUpload.push_back(img);
		else
			img->LoadGPU();
	}

	static void add(animation* anim) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (active && !isOnMainThread())
			animationsToUpload.push_back(anim);
	}

	static void add(text* text) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (active && !isOnMainThread())
			textToUpload.push_back(text);
		else
			text->LoadGPU();
	}

	static void add(URectangle* rect) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (active && !isOnMainThread())
			rectToUpload.push_back(rect);
		else
			rect->LoadGPU();
	}

	static void remove(Image* img) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		// list shouldn't contain anything
		if (!active)
			return;

		auto it = std::find(imagesToUpload.begin(), imagesToUpload.end(), img);
		if (it != imagesToUpload.end())
			imagesToUpload.erase(it);
	}

	static void remove(animation* anim) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		// list shouldn't contain anything
		if (!active)
			return;

		auto it = std::find(animationsToUpload.begin(), animationsToUpload.end(), anim);
		if (it != animationsToUpload.end())
			animationsToUpload.erase(it);
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

	static void remove(URectangle* rect) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		// list shouldn't contain anything
		if (!active)
			return;

		auto it = std::find(rectToUpload.begin(), rectToUpload.end(), rect);
		if (it != rectToUpload.end())
			rectToUpload.erase(it);
	}

	static void close() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		active = false;
	}

	static void LoadAllGPUData() {
		for (int i = 0; i < imagesToUpload.size(); i++) {
			imagesToUpload[i]->LoadGPU();
		}
		for (int i = 0; i < animationsToUpload.size(); i++) {
			animationsToUpload[i]->setQueuedAnim();
			animationsToUpload[i]->playQueuedStart();
		}
		for (int i = 0; i < textToUpload.size(); i++) {
			textToUpload[i]->LoadGPU();
		}
		for (int i = 0; i < rectToUpload.size(); i++) {
			rectToUpload[i]->LoadGPU();
			rectToUpload[i]->updatePositionsList();
		}

		imagesToUpload.clear();
		animationsToUpload.clear();
		textToUpload.clear();
		rectToUpload.clear();
	}

private:
	static inline std::vector<Image*> imagesToUpload;
	static inline std::vector<animation*> animationsToUpload;
	static inline std::vector<text*> textToUpload;
	static inline std::vector<URectangle*> rectToUpload;
	static inline bool active = false;
	static inline std::recursive_mutex mutex;
	static inline std::thread::id mainThreadId;
};