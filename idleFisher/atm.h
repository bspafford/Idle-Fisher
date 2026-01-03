#pragma once

#include "npc.h"

class UcurrencyConverterWidget;
class Timer;

class Aatm : public npc {
public:
	Aatm(vector loc);
	~Aatm();

	void draw(Shader* shaderProgram) override;

	void click() override;

	//void startConversion(FcurrencyStruct* currency, FsaveCurrencyStruct* saveCurrency, FcurrencyConversionStruct* currencyConversionStruct);
	//void stopConversion(FcurrencyStruct* currency, FsaveCurrencyStruct* saveCurrency, FcurrencyConversionStruct* currencyConversionStruct);
	//void finishConversion(int id);
	//void calcIdleProfits(float timeDiff);
private:
	std::unique_ptr<UcurrencyConverterWidget> currencyConverterWidget;

	//std::vector<DeferredPtr<Timer>> conversionTimers;
};