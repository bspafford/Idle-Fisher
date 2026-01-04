#pragma once

#include "widget.h"
#include "text.h"
#include "button.h"

class URectangle;

class ConfirmWidget : public widget {
public:
	ConfirmWidget(widget* parent);

	void draw(Shader* shader) override;

	virtual void setLoc(vector loc) override;

	template <class T> void AddSaveCallback(T* const object, void (T::* const callback) ()) {
		saveButton->addCallback(object, callback);
	}
	template <class T> void AddRevertCallback(T* const object, void (T::* const callback) ()) {
		revertButton->addCallback(object, callback);
	}
	template <class T> void AddCancelCallback(T* const object, void (T::* const callback) ()) {
		cancelButton->addCallback(object, callback);
	}

private:
	virtual void setupLocs() override;

	std::function<void()> saveCallback;
	std::function<void()> revertCallback;
	std::function<void()> cancelCallback;

	std::unique_ptr<Image> backgroundImage;
	std::unique_ptr<text> saveText;
	std::unique_ptr<Ubutton> saveButton;
	std::unique_ptr<Ubutton> revertButton;
	std::unique_ptr<Ubutton> cancelButton;
	std::unique_ptr<text> saveButtonText;
	std::unique_ptr<text> revertButtonText;
	std::unique_ptr<text> cancelButtonText;
	std::unique_ptr<URectangle> rect;
};