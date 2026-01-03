#pragma once

#include <unordered_set>

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
	DeferredPtr() : ptr(nullptr) {}

	DeferredPtr(CreateDeferred<T>&& cd) noexcept : ptr(cd.ptr) { cd.ptr = nullptr; }

	~DeferredPtr() { if (ptr) DeferredDelete(ptr); }

	T* operator->() const { return ptr; }
	T& operator*() const { return *ptr; }
	DeferredPtr<T>& operator=(CreateDeferred<T>&& other) noexcept {
		if (ptr) DeferredDelete(ptr); // delete if already created
		ptr = other.ptr;
		other.ptr = nullptr; // transfer ownership
		AddObject(ptr); // add to instance list
		return *this;
	}
	explicit operator bool() const { return ptr != nullptr; }
	bool operator!() const { return ptr == nullptr; }

private:
	T* ptr = nullptr;

	static void DeferredDelete(T* obj);

	static void AddObject(T* obj) {
		static std::unordered_set<T*>& list = GetInstanceList();
		list.insert(obj);
	}

public:
	static std::unordered_set<T*>& GetInstanceList() {
		static std::unordered_set<T*> instances;
		return instances;
	}

	static std::unordered_set<T*>& GetDeferredList() {
		static std::unordered_set<T*> deferred;
		return deferred;
	}

	static void FlushDeferred() {
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
};