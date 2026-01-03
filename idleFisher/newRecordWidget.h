#pragma once

#include "widget.h"
#include "deferredPtr.h"

class text;
class Timer;

class UnewRecordWidget : public widget {
public:
	UnewRecordWidget(widget* parent);
	~UnewRecordWidget();
	void draw(Shader* shaderProgram);
	void start(int fishSize);
	void finished();

private:
	std::unique_ptr<text> recordText;
	DeferredPtr<Timer> startTimer;
};