#include "deferredPtr.h"
#include "main.h"

template <typename T>
void DeferredPtr<T>::DeferredDelete(T* obj) {
	if (Main::IsRunning()) {
		static std::unordered_set<T*>& list = GetDeferredList();
		list.insert(obj);
	} else { // otherwise delete now because program is closing
		delete obj;
	}
}