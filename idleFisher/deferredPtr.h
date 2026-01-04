#pragma once

#include <vector>
#include <iostream>
#include <mutex>

template <typename T>
class DeferredPtr;

template <typename T>
class CreateDeferred {
public:
	template <typename... Args>
	explicit CreateDeferred(Args&&... args) : ptr(new T(std::forward<Args>(args)...)) {}
	~CreateDeferred() { if (ptr) delete ptr; } // delete if still has ownership
	friend class DeferredPtr<T>;
private:
	T* ptr;
};

template <typename T>
class DeferredPtr {
public:
	// default constructor
	DeferredPtr() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		ptr = nullptr;
	}

	DeferredPtr(CreateDeferred<T>&& cd) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		ptr = cd.ptr;
		CreateNew(cd);
	}

	~DeferredPtr() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (ptr) DeferredDelete(ptr);
	}

	T* operator->() const { return ptr; }
	T& operator*() const { return *ptr; }
	DeferredPtr<T>& operator=(CreateDeferred<T>&& other) noexcept {
		return CreateNew(other);
	}
	explicit operator bool() const { return ptr != nullptr; }
	bool operator!() const { return ptr == nullptr; }

private:
	static inline std::recursive_mutex mutex;
	static inline bool shuttingDown = false;
	T* ptr = nullptr;

	DeferredPtr<T>& CreateNew(CreateDeferred<T>& other) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (ptr) DeferredDelete(ptr); // delete if already created
		ptr = other.ptr;
		other.ptr = nullptr; // transfer ownership
		AddObject(ptr); // add to instance list
		return *this;
	}

	static void DeferredDelete(T* obj) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (!shuttingDown) {
			static std::vector<T*>& list = GetDeferredList();
			list.push_back(obj);
		} else { // otherwise delete now because program is closing
			delete obj;
		}
	}

	static void AddObject(T* obj) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		static std::vector<T*>& instances = GetInstanceList();

		auto it = std::find(instances.begin(), instances.end(), obj);
		if (it != instances.end())
			std::cout << "obj already in list\n";

		auto& deferred = GetDeferredList();
		auto it1 = std::find(deferred.begin(), deferred.end(), obj);
		if (it1 != deferred.end())
			std::cout << "obj is about to be deleted\n";

		instances.push_back(obj);
	}

	static std::vector<T*>& GetInstanceList() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		static std::vector<T*> instances;
		return instances;
	}

	static std::vector<T*>& GetDeferredList() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		static std::vector<T*> deferred;
		return deferred;
	}

public:
	static std::vector<T*> GetInstanceListVal() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return GetInstanceList();
	}

	static void FlushDeferred() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		auto& instances = GetInstanceList();
		auto& deferred = GetDeferredList();
		for (T* obj : deferred) {
			// remove object from instance list
			auto it = std::find(instances.begin(), instances.end(), obj);
			if (it != instances.end())
				instances.erase(it);
			delete obj; // delete object
		}
		deferred.clear();
	}

	// When program starts to close
	static void BeginShutdown() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		shuttingDown = true;
	}
};