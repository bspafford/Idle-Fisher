#pragma once

#include "widget.h"

class text;
class Ubutton;

class UsettingsBlock : public widget {
public:
	UsettingsBlock(widget* parent, std::string optionsName, int length, std::vector<std::string> options, bool* settingsValue);

	void draw(Shader* shaderProgram) override;

	void setLoc(vector loc) override;

	template <class T>
	void addCallback(T* const object, void (T::* const callback) ()) {
		callback_ = std::bind_front(callback, object);
	}
	void addCallback(void (*callback) ());

	void toggleOption();
	std::string getSelectedOption();

	void UpdateValue();

private:
	void SetValue(int value);

	std::vector<std::string> options;
	std::unique_ptr<text> selectedOptionText;

	std::unique_ptr<text> optionsTitle;
	std::unique_ptr<Ubutton> optionsButton;

	std::function<void()> callback_;

	int currOptionsIdx = 0;
	bool* settingsValue;
};