#pragma once

#include <string>
#include "saveData.h"

class shortNumbers {
public:
	static std::string convert2Short(double num, bool showDecimals = false) {
		if (std::isinf(num)) {
			std::cerr << "convert2Short: val is inf\n";
			abort();
		}

		if (SaveData::settingsData.shortNumbers) {
			if (num < 1000) {
				if (!showDecimals)
					return std::to_string((int)num);
				else {
					return formateStringFromDouble(num);
				}
			}

			double test = num;
			int counter = 0;
			while (test >= 1000) {
				test /= 1000;
				counter++;
			}

			std::string numSuffix = "";
			switch (counter) {
			case 0:
				break;
			case 1:
				numSuffix = "k";
				break;
			case 2:
				numSuffix = "m";
				break;
			case 3:
				numSuffix = "b";
				break;
			case 4:
				numSuffix = "t";
				break;
			case 5:
				numSuffix = "q";
				break;
			case 6:
				numSuffix = "Q";
				break;
			case 7:
				numSuffix = "s";
				break;
			case 8:
				numSuffix = "S";
				break;
			case 9:
				numSuffix = "o";
				break;
			case 10:
				numSuffix = "n";
				break;
			case 11:
				numSuffix = "d";
				break;
			case 12:
				numSuffix = "U";
				break;
			case 13:
				numSuffix = "D";
				break;
			case 14:
				numSuffix = "T";
				break;
			case 15:
				numSuffix = "Qt";
				break;
			case 16:
				numSuffix = "Qd";
				break;
			case 17:
				numSuffix = "Sd";
				break;
			case 18:
				numSuffix = "St";
				break;
			case 19:
				numSuffix = "O";
				break;
			case 20:
				numSuffix = "N";
				break;
			case 21:
				numSuffix = "v";
				break;
			case 22:
				numSuffix = "c";
				break;
			case 23:
				numSuffix = "Dvg";
				break;
			case 24:
				numSuffix = "Tvg";
				break;
			case 25:
				numSuffix = "Qav";
				break;
			case 26:
				numSuffix = "Qvg";
				break;
			case 27:
				numSuffix = "Svg";
				break;
			case 28:
				numSuffix = "Spv";
				break;
			case 29:
				numSuffix = "Ovg";
				break;
			case 30:
				numSuffix = "Nvg";
				break;
			case 31:
				numSuffix = "Tg";
				break;
			}

			test = round(test * 100);
			std::string test1 = "";
			if (num >= 1000) {
				if ((int)test % 100 == 0)
					test1 = ".00";
				else if ((int)test % 10 == 0)
					test1 = "0";
			}

			test /= 100;

			/*
		// format string correctly
			std::string string = std::to_string(test);

			// has to have three chars total (not including the .
			std::string newString = "";
			// then get the first three char
			for (int i = 0; i < string.size(); i++) {
				if ((i < 3 && newString.find('.') == std::string::npos) || (i < 4 && newString.find('.') != std::string::npos))
					newString.append(std::string(1, string[i]));
				else
					break;
			}

			std::string addedString = "";
			if (newString.find('.') != std::string::npos) { // then it can be 4
				if (newString.size() == 3)
					addedString = "0";
			} else if (newString.size() == 2)
				addedString = "0";
				*/

			return formateStringFromDouble(test) + numSuffix; // + test1 + numSuffix;
		} else {
			return std::format("{:.3g}", num);
		}
	}

private:
	// formates the string to have 3 numbers not including the "." example: 1.00, 21.4, 121, etc.
	static std::string formateStringFromDouble(double num) {
		std::string string = std::to_string(num);

		std::string newString = "";
		// then get the first three char
		for (int i = 0; i < string.size(); i++) {
			if ((i < 3 && newString.find('.') == std::string::npos) || (i < 4 && newString.find('.') != std::string::npos))
				newString.append(std::string(1, string[i]));
			else
				break;
		}

		std::string addedString = "";
		if (newString.find('.') != std::string::npos) { // then it can be 4
			if (newString.size() == 3)
				addedString = "0";
		} else if (newString.size() == 2)
			addedString = "0";

		return newString + addedString;
	}

};